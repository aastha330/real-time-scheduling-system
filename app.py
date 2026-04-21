import time
import json
import subprocess
from flask import Flask, render_template, request, Response, jsonify

app = Flask(__name__)

# Protocol settings 
PROTOCOLS = {
    'no': {
        'name': 'No Protocol',
        'exec_time': 5.0,
        'explanation': 'Priority inversion occurs. High-priority task is delayed significantly because a lower-priority task holds the required resource.',
        'status': 'Error',
        'script': 'scripts/sim_no.py'
    },
    'pip': {
        'name': 'Priority Inheritance Protocol',
        'exec_time': 3.0,
        'explanation': 'Priority inheritance reduces delay. Task L temporarily inherits Task H\'s priority, preventing Task M from preempting.',
        'status': 'Warning',
        'script': 'scripts/sim_pip.py'
    },
    'pcp': {
        'name': 'Priority Ceiling Protocol',
        'exec_time': 1.5,
        'explanation': 'Priority ceiling prevents inversion completely. System dynamically adjusts priority limits to guarantee safe preemption without deadlocks.',
        'status': 'Success',
        'script': 'scripts/sim_pcp.py'
    }
}
# This section defines scheduling protocols and their execution behavior
@app.route('/')
def index():
    return render_template('index.html')

@app.route('/run/<protocol>')
def run_protocol(protocol):
    if protocol not in PROTOCOLS:
        return jsonify({'error': 'Invalid protocol'}), 400
    
    data = PROTOCOLS[protocol]
    script_path = data['script']
    
    def generate():
        # Yield metadata first (optional, but let's just wait until the end or send init state)
        yield f"data: {json.dumps({'type': 'init', 'name': data['name']})}\n\n"
        
        # Start subprocess
        process = subprocess.Popen(['python', script_path], stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
        
        # Stream output line by line
        for line in iter(process.stdout.readline, ''):
            if line:
                yield f"data: {json.dumps({'type': 'log', 'data': line.strip()})}\n\n"
                
        process.stdout.close()
        process.wait()
        
        # Send complete event with metadata
        yield f"data: {json.dumps({'type': 'complete', 'time': data['exec_time'], 'explanation': data['explanation'], 'status': data['status'], 'protocol_key': protocol})}\n\n"

    return Response(generate(), mimetype='text/event-stream')

@app.route('/comparison')
def comparison():
    return render_template('comparison.html')

@app.route('/about')
def about():
    return render_template('about.html')

@app.route('/results')
def results():
    protocol = request.args.get('protocol', 'no')
    delay = request.args.get('delay', '')
    low = request.args.get('low', '')
    medium = request.args.get('medium', '')
    high = request.args.get('high', '')
    return render_template('results.html', protocol=protocol, delay=delay, low=low, medium=medium, high=high)

@app.route('/download')
def download():
    protocol = request.args.get('protocol', 'no')
    if protocol not in PROTOCOLS:
        return "Invalid protocol", 400
    
    data = PROTOCOLS[protocol]
    content = f"--- Real-Time Scheduling System Results ---\nProtocol: {data['name']}\nExecution Time: {data['exec_time']}s\nExplanation: {data['explanation']}\n\n--- Note: Live simulation logs are not persisted down to the file level dynamically in this route yet. Run the simulation on the dashboard! ---\n"
    
    return Response(content, mimetype='text/plain', headers={"Content-disposition": f"attachment; filename=results_{protocol}.txt"})

if __name__ == '__main__':
    app.run(debug=True, port=5000)
