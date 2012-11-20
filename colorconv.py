from bs4 import BeautifulSoup
import sys
import json

soup = BeautifulSoup(open(sys.argv[1]).read())

output = []

for tag in soup.findAll('tr'):
    try:
        td = tag.findAll('td')
        id = td[0].text
        name = td[1].text
        red = int(td[2].text)
        green = int(td[3].text)
        blue = int(td[4].text)
    except:
        continue

    output.append({'id': id, 'name': name, 'red': red, 'green': green, 'blue': blue})

print json.dumps(output)
