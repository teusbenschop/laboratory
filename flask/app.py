from flask import Flask, request, jsonify, json, send_file, send_from_directory, render_template
from flask_cors import CORS
import requests
import os
import base64
import shutil
import time
import uuid
import glob
import datetime
import io
import sys

app = Flask(__name__)
CORS(app)

@app.route('/', methods=['GET'])
def index():
    return render_template('index.html')


