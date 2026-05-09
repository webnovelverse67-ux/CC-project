document.addEventListener('DOMContentLoaded', () => {
    const compileBtn = document.getElementById('compile-btn');
    const sourceCode = document.getElementById('source-code');
    const targetLang = document.getElementById('target-lang');

    // Tabs
    const tabBtns = document.querySelectorAll('.tab-btn');
    const tabPanes = document.querySelectorAll('.tab-pane');

    tabBtns.forEach(btn => {
        btn.addEventListener('click', () => {
            tabBtns.forEach(b => b.classList.remove('active'));
            tabPanes.forEach(p => p.classList.remove('active'));

            btn.classList.add('active');
            const targetId = btn.getAttribute('data-target');
            document.getElementById(targetId).classList.add('active');
        });
    });

    compileBtn.addEventListener('click', async () => {
        const code = sourceCode.value;
        const lang = targetLang.value;

        // Reset outputs
        document.getElementById('runtime-out').innerText = 'Compiling...';
        document.getElementById('target-code-out').innerText = '';
        document.getElementById('ast-out').innerText = '';
        document.getElementById('symtab-out').innerText = '';
        document.getElementById('semantic-out').innerText = '';
        document.getElementById('tac-out').innerText = '';

        try {
            const response = await fetch('http://localhost:5000/compile', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json'
                },
                body: JSON.stringify({
                    sourceCode: code,
                    targetLanguage: lang
                })
            });

            const data = await response.json();

            if (response.ok) {
                // Populate Output Tab
                document.getElementById('runtime-out').innerText = data.runtime_output || 'No output.';
                document.getElementById('target-code-out').innerText = data.target_code || '';

                // Populate Parse Tree
                document.getElementById('ast-out').innerText = JSON.stringify(data.ast, null, 2);

                // Populate Symbol Table
                document.getElementById('symtab-out').innerText = JSON.stringify(data.symbol_table, null, 2);

                // Populate Semantic Analysis (Errors)
                if (data.errors && data.errors.length > 0) {
                    document.getElementById('semantic-out').innerText = JSON.stringify(data.errors, null, 2);
                    document.getElementById('semantic-out').style.color = '#ff6b6b';
                } else {
                    document.getElementById('semantic-out').innerText = 'No semantic errors detected.';
                    document.getElementById('semantic-out').style.color = '#8ce99a';
                }

                // Populate TAC
                document.getElementById('tac-out').innerText = JSON.stringify(data.tac, null, 2);

            } else {
                document.getElementById('runtime-out').innerText = `Error: ${data.error}\n\nDetails:\n${data.details || data.raw_output || data.stderr || ''}`;
            }

        } catch (error) {
            document.getElementById('runtime-out').innerText = `Network/Fetch Error: ${error.message}`;
        }
    });
});