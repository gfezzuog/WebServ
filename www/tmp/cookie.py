from flask import Flask, render_template, request, redirect, url_for

app = Flask(__name__)

@app.route('/post_test', methods=['GET'])
def post_test():
    if request.args.get('si') and 'lname' in request.cookies and 'fname' in request.cookies:
        response = redirect(url_for('index'))
        response.delete_cookie('lname')
        response.delete_cookie('fname')
        return response

    if 'lname' in request.cookies and 'fname' in request.cookies:
        return render_template('welcome.html', lname=request.cookies.get('lname'), fname=request.cookies.get('fname'))
    else:
        return render_template('not_set.html')

if __name__ == '__main__':
    app.run(debug=True)