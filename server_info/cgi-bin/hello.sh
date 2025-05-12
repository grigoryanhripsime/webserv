#!/bin/sh
# Parse QUERY_STRING
name=$(echo "$QUERY_STRING" | grep -o 'name=[^&]*' | cut -d= -f2 | sed 's/%20/ /g')
if [ -z "$name" ]; then
    name="Guest"
fi

# Output CGI headers
echo "Content-Type: text/html"
echo "Connection: close"
echo ""

# Output HTML
cat << EOF
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Hello from Shell</title>
    <style>
        body {
            background-color: #e8f5e9;
            font-family: Arial, sans-serif;
            text-align: center;
            margin-top: 50px;
        }
        .container {
            background-color: #ffffff;
            padding: 20px;
            border-radius: 10px;
            display: inline-block;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>Hello, $name!</h1>
        <p>This is a Shell CGI script.</p>
        <form method="GET" action="/cgi-bin/hello.sh">
            <input type="text" name="name" placeholder="Enter your name">
            <button type="submit">Submit</button>
        </form>
    </div>
</body>
</html>
EOF