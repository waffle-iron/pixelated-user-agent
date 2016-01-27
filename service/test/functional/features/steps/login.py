#
# Copyright (c) 2016 ThoughtWorks, Inc.
#
# Pixelated is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Pixelated is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with Pixelated. If not, see <http://www.gnu.org/licenses/>.
from behave import when, then
from common import *


@when(u'I open the login page')
def login_page(context):
    context.browser.get(MULTI_USER_URL + '/login')


@when(u'I enter {username} and {password} as credentials')
def enter_credentials(context, username, password):
    fill_by_css_selector(context, 'input#email', username)
    fill_by_css_selector(context, 'input#password', password)


@when(u'I click on the login button')
def click_login(context):
    login_button = wait_until_element_is_visible_by_locator(context, (By.CSS_SELECTOR, 'input[name="login"]'))
    login_button.click()
