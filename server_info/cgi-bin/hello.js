#!/usr/bin/node
const url = require('url');
const querystring = require('querystring');

// Get query parameters from REQUEST_URI
const requestUri = process.env.REQUEST_URI || '';
const parsedUrl = url.parse(requestUri);
const query = querystring.parse(parsedUrl.query || '');
const name = query.name || 'Guest';

// Output CGI headers
console.log('Content-Type: text/html');
console.log('Connection: close');
console.log('');

// Output HTML
console.log(`
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Hello from JavaScript</title>
    <style>
        body {
            background-color: #e0f7fa;
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
        <h1>Hello, ${name}!</h1>
        <p>This is a JavaScript CGI script.</p>
        <form method="GET" action="/cgi-bin/hello.js">
            <input type="text" name="name" placeholder="Enter your name">
            <button type="submit">Submit</button>
        </form>
    </div>
</body>
</html>
`);