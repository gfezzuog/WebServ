from flask import Flask, request, render_template, redirect, url_for

app = Flask(__name__)

@app.route('/')
def index():
    return 'Hello, World!'

@app.route('/cookie', methods=['GET', 'POST'])
def cookie():
    if request.method == 'POST':
        lname = request.form.get('lname')
        fname = request.form.get('fname')

        if lname and fname:
            # Set cookies
            response = redirect(url_for('see_cookie'))
            response.set_cookie('lname', lname, max_age=30*24*60*60)
            response.set_cookie('fname', fname, max_age=30*24*60*60)
            return response

    return render_template('cookie.html')

@app.route('/see_cookie')
def see_cookie():
    lname = request.cookies.get('lname')
    fname = request.cookies.get('fname')

    return f'Last name: {lname}<br>First name: {fname}'

if __name__ == '__main__':
    app.run(debug=True)
