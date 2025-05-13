#!/usr/bin/python3
import cgi
import cgi

# Parse form data
form = cgi.FieldStorage()
name = form.getvalue('name', 'Guest')

# Output CGI headers
print('Content-Type: text/html')
print('Connection: close')
print('')

# Output HTML
print(f'''
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Hello from Python</title>
    <style>
        body {{
            background-color: #fff3e0;
            font-family: Arial, sans-serif;
            text-align: center;
            margin-top: 50px;
        }}
        .container {{
            background-color: #ffffff;
            padding: 20px;
            border-radius: 10px;
            display: inline-block;
        }}
    </style>
</head>
<body>
    <div class="container">
        <h1>Hello, {name}!</h1>
        <p>This is a Python CGI script.</p>
        <form method="GET" action="/cgi-bin/hello.py">
            <input type="text" name="name" placeholder="Enter your name">
            <button type="submit">Submit</button>
        </form>
    </div>
</body>
</html>
''')