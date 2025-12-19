#include <windows.h>
#include <wincrypt.h>
#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <sstream>

#pragma comment(lib, "crypt32.lib")

using namespace std;

// Função para converter bytes do Thumbprint em String Hexadecimal [cite: 69]
string binToHex(BYTE* data, DWORD len) {
    stringstream ss;
    for (DWORD i = 0; i < len; i++)
        ss << hex << setw(2) << setfill('0') << (int)data[i];
    return ss.str();
}

void enviarMensagem(string json) {
    unsigned int len = json.length();
    // Protocolo Native Messaging: 4 bytes de tamanho + mensagem [cite: 41]
    cout.write((char*)&len, 4);
    cout << json << flush;
}

int main() {
    HCERTSTORE hStore = CertOpenStore(CERT_STORE_PROV_SYSTEM, 0, NULL, CERT_SYSTEM_STORE_CURRENT_USER, L"My");
    if (!hStore) return 1;

    string jsonResponse = "[";
    PCCERT_CONTEXT pCert = NULL;
    bool primeiro = true;

    while (pCert = CertFindCertificateInStore(hStore, X509_ASN_ENCODING, 0, CERT_FIND_ANY, NULL, pCert)) {
        if (!primeiro) jsonResponse += ",";
        primeiro = false;

        // Extração de metadados
        char subject[256], issuer[256];
        CertGetNameStringA(pCert, CERT_NAME_SIMPLE_DISPLAY_TYPE, 0, NULL, subject, 256);
        CertGetNameStringA(pCert, CERT_NAME_SIMPLE_DISPLAY_TYPE, CERT_NAME_ISSUER_FLAG, NULL, issuer, 256);

        // Thumbprint
        BYTE hash[20]; DWORD cbHash = 20;
        CertGetCertificateContextProperty(pCert, CERT_HASH_PROP_ID, hash, &cbHash);

        // Chave Privada
        DWORD cbData = 0;
        bool hasKey = CertGetCertificateContextProperty(pCert, CERT_KEY_PROV_INFO_PROP_ID, NULL, &cbData);

        jsonResponse += "{\"subject\":\"" + string(subject) + "\",";
        jsonResponse += "\"issuer\":\"" + string(issuer) + "\",";
        jsonResponse += "\"thumbprint\":\"" + binToHex(hash, cbHash) + "\",";
        jsonResponse += "\"hasPrivateKey\":" + string(hasKey ? "true" : "false") + "}";
    }

    jsonResponse += "]";
    enviarMensagem(jsonResponse);
    CertCloseStore(hStore, 0);
    return 0;
}