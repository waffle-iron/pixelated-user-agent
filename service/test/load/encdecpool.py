import json
import os

import time
import requests
from locust import HttpLocust, TaskSet, task
from random import randint
from requests.packages.urllib3.exceptions import InsecureRequestWarning

requests.packages.urllib3.disable_warnings(InsecureRequestWarning)

LEAP_PROVIDER = os.environ.get('LEAP_PROVIDER', 'dev.pixelated-project.org')


class EncryptDecryptSync(TaskSet):
    def __init__(self, *args, **kwargs):
        super(EncryptDecryptSync, self).__init__(*args, **kwargs)
        self.cookies = {}
        self.email_id = ''
        self.username = 'load34'

    def on_start(self):
        self._update_cookies()

    def _regular_ajax_params(self):
        return {'cookies': self.cookies, 'timeout': 240,
                'headers': {'X-Requested-With': 'XMLHttpRequest',
                            'X-XSRF-TOKEN': self.cookies.get('XSRF-TOKEN', '')}}

    def _update_cookies(self):
        self.cookies.update(self.client.get("/", verify=False).cookies.get_dict())
        while not self.cookies.get('XSRF-TOKEN', ''):
            time.sleep(1)
            self.cookies.update(self.client.get("/", verify=False).cookies.get_dict())

    @task
    def send_mail(self):
        random_number = randint(1, 500000)
        payload = {
            "tags": ["drafts"],
            "body": "some text lorem ipsum %d" % random_number,
            "attachments": [],
            "ident": "",
            "header": {
                "to": ["%s@%s" % (self.username, LEAP_PROVIDER)],
                "cc": [],
                "bcc": [],
                "subject": "load testing %d" % random_number}}

        email_response = self.client.post('/mails', json=payload, name="/send_mail", **self._regular_ajax_params())
        if email_response.status_code == 201:
            email_id = json.loads(email_response.content)['ident']
            if self.read_mail(email_id):
                self.delete_mail(email_id)

    def delete_mail(self, email_id=None):
        payload = {"idents": [email_id]}
        move_to_trash = self.client.post('/mails/delete', json=payload, name="/move_to_trash",
                                         **self._regular_ajax_params())
        if move_to_trash.status_code == 200:
            self.client.post('/mails/delete', json=payload, name="/delete_permanently", **self._regular_ajax_params())

    def read_mail(self, email_id=None):
        response = self.client.get('/mail/%s' % email_id, verify=False, name="/retrieve_mail")
        return response.status_code == 200


class SingleUserMode(HttpLocust):
    task_set = EncryptDecryptSync
    min_wait = 1000
    max_wait = 3000
