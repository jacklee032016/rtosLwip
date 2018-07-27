import requests
import json

url = 'http://192.168.166.2/x-nmos/node/v1.2/self'
response = requests.get(url, data=None)

print("code:"+ str(response.status_code))
print("******************")
print("headers:"+ str(response.headers))
print("******************")

resBytes = response.content;
resStr = resBytes.decode("utf-8")


    # Loading the response data into a dict variable
    # json.loads takes in only binary or string variables so using content to fetch binary content
    # Loads (Load String) takes a Json file and converts into python data structure (dict or list, depending on JSON)
    
try:
    jData = json.loads(resStr)

    print("The response contains {0} properties".format(len(jData)))
    if(response.ok):
        print('raw "%s"\n'%(json.dumps(jData, indent=4, sort_keys=True)))
    else:
        print("\n")
        for key in jData:
            print(key + " : " + str(jData[key]))
except (TypeError, ValueError):
    raise Exception("parse JSON string failed \"%s\""%(resStr))
        
#    response.raise_for_status()
    