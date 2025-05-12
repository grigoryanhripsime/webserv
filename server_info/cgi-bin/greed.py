#!/usr/bin/env python3
"""
A pretty Python CGI script for a custom NGINX-like web server.
Displays a styled greeting form, accepts 'name' via GET/POST.
No cookie handling, as requested.
Accessible at /cgiFolder/greet.py.
"""

import cgi
import os
import logging
import sys

# Configure logging
logging.basicConfig(
    filename="/home/tigpetro/Desktop/webserv/server_info/cgi-bin/cgi.log",
    level=logging.INFO,
    format="%(asctime)s - %(levelname)s - %(message)s"
)

def get_form_data():
    """Retrieve form data from GET or POST."""
    form = cgi.FieldStorage()
    return form.getvalue("name", "").strip()

def generate_html(name):
    """Generate styled HTML output."""
    greeting = f"Hello, {name}!" if name else "Hello, Guest!"
    return f"""<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>CGI Greeting</title>
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.3/dist/css/bootstrap.min.css" rel="stylesheet">
    <link rel="icon" type="image/png" sizes="32x32" href="https://static.vecteezy.com/system/resources/thumbnails/029/570/400/small/cute-teddy-bear-ai-generative-free-png.png">
    <style>
        body {{
            background-color: #f8f9fa;
            font-family: Arial, sans-serif;
        }}
        .container {{
            max-width: 600px;
            margin-top: 50px;
        }}
        .card {{
            border-radius: 10px;
            box-shadow: 0 4px 8px rgba(0,0,0,0.1);
        }}
    </style>
</head>
<body>
    <div class="container">
        <div class="card p-4">
            <h1 class="text-center mb-4">{greeting}</h1>
            <form method="POST" action="/cgiFolder/greet.py" class="mb-3">
                <div class="input-group">
                    <input type="text" name="name" class="form-control" placeholder="Enter your name" value="{name}">
                    <button type="submit" class="btn btn-primary">Greet</button>
                </div>
            </form>
            <p class="text-center text-secondary">Powered by Python CGI</p>
        </div>
    </div>
</body>
</html>
"""

def main():
    """Main CGI handler."""
    try:
        # Parse form data
        name = get_form_data()

        # Output headers
        print("Content-Type: text/html\n")

        # Output HTML
        print(generate_html(name))

    except Exception as e:
        logging.error(f"CGI error: {e}")
        print("Content-Type: text/html\n")
        print("<html><body><h1>Internal Server Error</h1></body></html>")
        sys.exit(1)

if __name__ == "__main__":
    main()