import requests
import json
import boto3
import time
from boto3.dynamodb.types import TypeDeserializer, TypeSerializer

client = boto3.client('dynamodb')
dynamodb = boto3.resource('dynamodb')  # Create a DynamoDB resource
iot_client = boto3.client('iot-data')



def PythonToDB(python_obj: dict) -> dict:
    serializer = TypeSerializer()
    return {
        k: serializer.serialize(v)
        for k, v in python_obj.items()
    }

def DBToPython(dynamo_obj: dict) -> dict:
    deserializer = TypeDeserializer()
    return {
        k: deserializer.deserialize(v) 
        for k, v in dynamo_obj.items()
    }

def get_user_info(access_token):
    user_info_url = 'https://dev-getq0rx4erbnc6mi.us.auth0.com/userinfo'
    headers = {'Authorization': f'{access_token}'}
    while True:
        response = requests.get(user_info_url, headers=headers)
        if response.status_code == 200:
            user_info = response.json()
            return user_info
        elif response.status_code == 429:  # Rate limit exceeded
            print("Rate limit exceeded. Retrying in 5 seconds...")
            time.sleep(5)
        else:
            print(f"Failed to get user info. Status code: {response.status_code}")
            print(response.text)
            break
       
def getUserId(access_token):
    user_info = get_user_info(access_token)
    email = user_info
    response = client.get_item(TableName='user_accounts', Key={"auth_id": {"S": email}})
    details = DBToPython(response['Item'])
    return details['user_id']
    

def lambda_handler(event, context):
    print("event = ", end="")
    print(event)
    if 'type' in event:
        typ = event.pop('type')
        if typ == 'config':
            switch_list = event.pop('device_list')
            fbdata={  "room": "",
                      "name": "",
                      "state": False}
            Id= event['id']
                      
            event.update({"name":"","roomHint":"","type":"",})
            for item in switch_list:
                event['Switch_id'] = item
                print(event)
                firebase_url = "https://homeautomated-b318b-default-rtdb.firebaseio.com/"+ Id +"/"+ item +"/.json"  #edit firebase link 
                response = requests.patch(firebase_url, json=fbdata)
                response = client.put_item(TableName='devicesAndUsers', Item=PythonToDB(event))# insertion itherem setttttt
        elif typ == 'action':
            #Switch_id = event.pop('Switch_id')
            response = client.update_item(
                TableName='devicesAndUsers',
                Key={"id": {"S": event['id']}, "Switch_id": {"S": event['Switch_id']}},
                UpdateExpression='SET stat = :val1',
                ExpressionAttributeValues={':val1': {'BOOL': event['stat']}}
            )
            Id = event['id']
            sid = event['Switch_id']
            firebase_url = f"https://homeautomated-b318b-default-rtdb.firebaseio.com/{Id}/{sid}/.json"
            fbdata = {
                    "code": sid,
                    "status": event['stat']
                    }
                    
            print(fbdata)
            response = requests.patch(firebase_url, json=fbdata)
            print(response)
           
    elif 'queryStringParameters' in event:
        query = event['queryStringParameters']
        typ=query['type']
        
            
        if typ=='action':
            if query['status']=='true':
                state=True
            else:
                state=False
            Swi=query['code']
            topic,Switc = Swi.split("-")
            message = {'Switch': Switc,
                                'stat':state
                      }
            responce=iot_client.publish(topic=topic, qos=0, payload=json.dumps(message))
            return "successes"
        elif typ=='rename':
            Id=query['id']
            data=json.loads(query['data'])
            sid=data['code']
            device=data['device']
            response = client.update_item(
                                            TableName='devicesAndUsers',
                                            Key={"id": {"S": Id},"Switch_id": {"S": data['code']}},
                                            UpdateExpression='SET #n = :val1, #r = :val2, #t = :val3',  # Update multiple attributes: #n, #r, and #t
                                            ExpressionAttributeValues={':val1': {'S': data['name']},
                                                                       ':val2': {'S': data['room']},
                                                                       ':val3': {'S': device}
                                            },
                                            ExpressionAttributeNames={'#n': 'name', '#r': 'roomHint', '#t': 'type'}  # Attribute names
                                        )
            firebase_url = f"https://homeautomated-b318b-default-rtdb.firebaseio.com/{Id}/{sid}/.json"
            fbdata = {
                    "code": sid,
                    "name": data['name'],
                    "room": data['room']
                    }
                    
            print(fbdata)
            response = requests.patch(firebase_url, json=fbdata)    
            return 'sucesses'
        elif typ == 'bluetooth':
            '''data=json.loads(query['data'])
            Id=query['id']
            response = client.query(
                                    TableName='devicesAndUsers',
                                    KeyConditionExpression='id = :partition_key_value',
                                    ExpressionAttributeValues={
                                        ':partition_key_value': {'S': Id}
                                    },
                                    ProjectionExpression='Switch_id'  # Specify the attribute you want to retrieve
                                    )
            macId=set()
            for item in response['Items']:
                mac,s=item['Switch_id']['S'].split("-")
                macId.add(mac)
            print(macId)
            for mac in macId:
                message = {'Switch': Switc,
                                'stat':state
                              }
                responce=iot_client.publish(topic=topic, qos=0, payload=json.dumps(message))
            # thalkalam ivide nikkette'''
            return "sucesses"
    else:
        access_token = event['headers']['authorization']
        #user_info = get_user_info(access_token)
        user_info = {'email':'akshayshaji02@gmail.com'}
        if user_info:
            print(user_info)
            print("email id", end="")
            print(user_info['email'])
            email = user_info['email']
            body = json.loads(event['body'])
            intent = body['inputs'][0]['intent']
            print(intent)
            if intent == "action.devices.SYNC":
                response = client.get_item(TableName='user_accounts', Key={"auth_id": {"S": email}})
                if 'Item' in response:
                    details = DBToPython(response['Item'])
                    userid = details['user_id']
                    data={"requestId": context.aws_request_id,
                        "payload": {"agentUserId": userid,"devices": []}}
                    
                    response = client.query(
                    TableName='devicesAndUsers',
                    KeyConditionExpression='id = :partition_key_value',
                    ExpressionAttributeValues={
                        ':partition_key_value': {'S': userid}
                    }
                )
                
                    print(response)
                    lst=[]
                    for item in response['Items']:
                        itm=DBToPython(item)
                        if 'name' in itm:
                            updat={
                               "id": itm['Switch_id'],
                               "name": {
                                "name": itm['name']
                               },
                               "roomHint": itm['roomHint'],
                               "traits": [
                                "action.devices.traits.OnOff"
                               ],
                               "type": itm['type'],
                               "willReportState": False}
                            lst.append(updat)
                    data["payload"]['devices']=lst
                    print("data t google = ",end="")
                    print(data)
                    return data  # innu itherem set akiiiiii
                else:
                    return "unauthorised"
            elif intent == "action.devices.QUERY":
                access_token = event['headers']['authorization']
                userid=getUserId(access_token)
                data = {"requestId": context.aws_request_id, "payload": {}}
                dev = {}
                devices = body['inputs'][0]['payload']['devices']
                for deviceID in devices:
                    ID = deviceID['id']
                    print(ID)
                    response = client.get_item(TableName='devicesAndUsers', Key={"id": {"S": userid},"Switch_id": {"S": ID}},ProjectionExpression="stat") #need to make this efficient in future
                    print(response)
                    dev.update({ID: {"on": response['Item']['stat']['BOOL'], "online": True, "status": "SUCCESS"}})
                data['payload'] = {'devices': dev}
                return data # ithum set akkiii( nee poli ahh)
            elif intent == "action.devices.EXECUTE":
                devices = body['inputs'][0]['payload']['commands'][0]
                state = devices['execution'][0]['params']['on']
                ret = {
                    "requestId": context.aws_request_id,
                    "payload": {'commands': [{'ids': [],
                                               "status": "SUCCESS",
                                               "states": {
                                                   "on": state,
                                                   "online": True
                                               }
                                           }
                                           ]
                               }
                }
                userid=getUserId(event['headers']['authorization'])
                for deviceID in devices['devices']:
                    ID = deviceID['id']
                    ret['payload']['commands'][0]['ids'].append(ID)
                    '''response = client.update_item(
                                TableName='devicesAndUsers',
                                Key={"id": {"S": userid},"Switch_id": {"S": ID}},
                                UpdateExpression='SET stat = :val1',
                                ExpressionAttributeValues={':val1': {'BOOL': state}}
                            )'''
                    topic,Switc = ID.split("-")
                    Switc=int(Switc)
                    message = {'Switch': Switc,
                                'stat':state
                              }
                    responce=iot_client.publish(topic=topic, qos=0, payload=json.dumps(message))
                    print(responce)
                print(ret)
                return ret
# google ssistant set nee oru killadi thanne