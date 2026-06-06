import os
import re

def build_webpage_header():
    print("--- AUTOMATIC WEB BUNDLER: Generating src/webpage.h ---")
    try:
        project_dir = os.path.dirname(os.path.abspath(__file__))
    except NameError:
        project_dir = os.getcwd()
        
    web_dir = os.path.join(project_dir, "src", "web")
    html_file = os.path.join(web_dir, "index.html")
    css_file = os.path.join(web_dir, "style.css")
    js_file = os.path.join(web_dir, "app.js")
    output_file = os.path.join(project_dir, "src", "webpage.h")

    # Garante que a pasta src/web/ exista
    if not os.path.exists(web_dir):
        os.makedirs(web_dir)
        print(f"Diretorio {web_dir} criado.")

    # Se os arquivos nao existirem, cria templates basicos vazios
    if not os.path.exists(html_file):
        with open(html_file, "w", encoding="utf-8") as f:
            f.write("<!DOCTYPE html>\n<html>\n<head>\n  <link rel=\"stylesheet\" href=\"style.css\">\n</head>\n<body>\n  <h1>Template</h1>\n  <script src=\"app.js\"></script>\n</body>\n</html>")
        print("Template index.html criado.")
    if not os.path.exists(css_file):
        with open(css_file, "w", encoding="utf-8") as f:
            f.write("/* CSS Personalizado */\nbody { background: #121212; color: #fff; }")
        print("Template style.css criado.")
    if not os.path.exists(js_file):
        with open(js_file, "w", encoding="utf-8") as f:
            f.write("// JS Personalizado\nconsole.log('Frontend pronto!');")
        print("Template app.js criado.")

    # Leitura dos arquivos
    with open(html_file, "r", encoding="utf-8") as f:
        html = f.read()

    with open(css_file, "r", encoding="utf-8") as f:
        css_content = f.read()

    with open(js_file, "r", encoding="utf-8") as f:
        js_content = f.read()

    # Substitui <link ... href="style.css" ...> pelo conteudo CSS inline
    html = re.sub(
        r'<link\s+[^>]*href=["\'].*?style\.css["\'][^>]*>',
        f'<style>\n{css_content}\n</style>',
        html,
        flags=re.IGNORECASE
    )

    # Substitui <script ... src="app.js" ...></script> pelo conteudo JS inline
    html = re.sub(
        r'<script\s+[^>]*src=["\'].*?app\.js["\'][^>]*>\s*</script>',
        f'<script>\n{js_content}\n</script>',
        html,
        flags=re.IGNORECASE
    )

    # Conteudo do arquivo header C++
    header_content = f"""#ifndef WEBPAGE_H
#define WEBPAGE_H

#include <Arduino.h>

// Arquivo gerado automaticamente pelo script pre_build_web.py.
// Nao edite este arquivo diretamente. Edite os arquivos em src/web/

const char webpage_html[] PROGMEM = R"rawliteral(
{html}
)rawliteral";

#endif // WEBPAGE_H
"""

    with open(output_file, "w", encoding="utf-8") as f:
        f.write(header_content)
    print("--- AUTOMATIC WEB BUNDLER: src/webpage.h gerado com sucesso! ---")

# Executa ao carregar o script
build_webpage_header()
