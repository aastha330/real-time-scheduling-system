import os
import re

base_html = """<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>{title}</title>
    <link rel="stylesheet" href="/style.css">
</head>
<body>
    <nav class="navbar">
        <a href="/" class="brand">
            <span>🚀 ASYS</span>
        </a>
        <div class="nav-links">
            <a href="/" class="{home_active}">Home</a>
            <a href="/comparison" class="{comp_active}">Comparison</a>
            <a href="/about" class="{about_active}">About</a>
        </div>
    </nav>

    <main class="container">
        {content}
    </main>

    <script>
        {scripts}
    </script>
</body>
</html>"""

def flatten(filename, title, home_active, comp_active, about_active):
    filepath = f"templates/{filename}"
    with open(filepath, 'r', encoding='utf-8') as f:
        content = f.read()
    
    # Extract block content
    content_match = re.search(r'{% block content %}(.*?){% endblock %}', content, re.DOTALL)
    content_text = content_match.group(1) if content_match else ""
    
    scripts_match = re.search(r'{% block scripts %}(.*?){% endblock %}', content, re.DOTALL)
    scripts_text = scripts_match.group(1) if scripts_match else ""
    
    if filename == "results.html":
        # Remove jinja variables inside results.html
        content_text = content_text.replace("{% if delay %}", "")
        content_text = content_text.replace("{% endif %}", "")
        content_text = content_text.replace("{{ delay }}", '<span id="p-delay"></span>')
        content_text = content_text.replace("{{ low }}", '<span id="p-low"></span>')
        content_text = content_text.replace("{{ medium }}", '<span id="p-medium"></span>')
        content_text = content_text.replace("{{ high }}", '<span id="p-high"></span>')
        
        # update the script to read from URL
        new_scripts = """
    const urlParams = new URLSearchParams(window.location.search);
    const protocol = urlParams.get('protocol') || 'no';
    const delay = urlParams.get('delay') || '0';
    const low = urlParams.get('low') || '0';
    const medium = urlParams.get('medium') || '0';
    const high = urlParams.get('high') || '0';
    
    document.getElementById('p-delay').innerText = delay;
    document.getElementById('p-low').innerText = low;
    document.getElementById('p-medium').innerText = medium;
    document.getElementById('p-high').innerText = high;
""" + scripts_text.replace('const protocol = "{{ protocol }}";', '')
        scripts_text = new_scripts
    
    # Use replace to avoid {} errors in format()
    html = base_html.replace("{title}", title).replace("{home_active}", home_active).replace("{comp_active}", comp_active).replace("{about_active}", about_active).replace("{content}", content_text).replace("{scripts}", scripts_text)
    
    with open(filepath, 'w', encoding='utf-8') as f:
        f.write(html)
    print(f"Built {filename}")

if __name__ == "__main__":
    os.system("git restore templates/index.html templates/comparison.html templates/about.html templates/results.html")
    flatten('index.html', 'Home | Real-Time Scheduler', 'active', '', '')
    flatten('comparison.html', 'Comparison | Real-Time Scheduler', '', 'active', '')
    flatten('about.html', 'About | Real-Time Scheduler', '', '', 'active')
    flatten('results.html', 'Results | Real-Time Scheduler', '', '', '')
