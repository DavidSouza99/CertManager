// Solicitar certificados ao abrir [cite: 93]
chrome.runtime.sendNativeMessage('com.bcc423.certificados', { cmd: "GET" }, (certs) => {
    const tabela = document.getElementById('tabelaCerts');
    chrome.storage.local.get(['selecionados'], (result) => {
        const salvos = result.selecionados || [];
        
        certs.forEach(c => {
            const row = tabela.insertRow();
            const isChecked = salvos.includes(c.thumbprint) ? 'checked' : '';
            row.innerHTML = `
                <td><input type="checkbox" class="cert-check" value="${c.thumbprint}" ${isChecked}></td>
                <td>${c.subject}</td>
                <td>${c.thumbprint.substring(0, 10)}...</td>
                <td>${c.hasPrivateKey ? 'Sim' : 'Não'}</td>
            `;
        });
    });
});

// Salvar seleção 
document.getElementById('btnSalvar').onclick = () => {
    const selecionados = Array.from(document.querySelectorAll('.cert-check:checked')).map(cb => cb.value);
    chrome.storage.local.set({ selecionados }, () => alert('Salvo!'));
};

document.getElementById('selectAll').onclick = (e) => {
    const checks = document.querySelectorAll('.cert-check');
    checks.forEach(cb => cb.checked = e.target.checked);
};