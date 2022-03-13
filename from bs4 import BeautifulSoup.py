from bs4 import BeautifulSoup
from selenium import webdriver

browser = webdriver.Firefox()
browser. get('http://웹페이지 주소')

soup = BeautifulSoup(browser.page_source, 'html.parser')
print(soup.find('h1').text)

import tensoflow as tf
