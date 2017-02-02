# Simple server script to handle donations
#
# mqtt structure:
# lisa/iot/donations/reader/<id>/
#                               /donate       <- incoming: tag id
#                               /user         -> outgoing: user name and new balance
#                               /setup/amount -> outgoing: amount of donation 
#########################################################################################
import paho.mqtt.client as mqtt
from tinydb import TinyDB, where, Query

def on_connect(client, userdata, flags, rc):
    print ("CONNACK received with code %d" % (rc))

def on_message(client, userdata, msg):
    pass
    # print(msg.topic+" "+str(msg.payload))

def on_message_donation_incoming(client, userdata, msg):
    topic = msg.topic
    tag = int(msg.payload)

    # get reader id from topic
    topics = topic.split('/')
    reader_pos = topics.index('reader') + 1
    reader_id = int(topics[reader_pos])

    # get entries from data base reader/room and update balance
    Reader = Query()
    reader = db_rooms.get(Reader.id == reader_id)
    User = Query()
    user = db_members.get(User.tag == tag)

    # update balances 
    new_user_balance  = user['balance'] - reader['donate_amount']
    new_room_balance = reader['balance'] + reader['donate_amount']

    # update data base entries
    db_rooms.update({'balance' : new_room_balance}, Reader.id == reader_id)
    db_members.update({'balance': new_user_balance}, User.tag == tag)

    # send data back to reader
    return_topic = "lisa/iot/donations/reader/"+str(reader_id)+"/user"
    return_payload = user['name']+" "+str(new_user_balance)
    client.publish(return_topic, return_payload) 


# setup mqtt client
client = mqtt.Client()
client.on_message = on_message
client.on_connect = on_connect

client.connect("localhost", 1883, 60)
client.subscribe("lisa/iot/donations/#")
client.message_callback_add("lisa/iot/donations/reader/+/donate", on_message_donation_incoming)

# create data base
db = TinyDB('./db.json')
# setup  data base tables
db_rooms = db.table('rooms')
db_members = db.table('members')

# create two rooms as example
db_rooms.purge() #start with fresh database 
db_rooms.insert({'id' : 1, 'name': 'werkstatt', 'balance': 0,  'donate_amount' : 1})
db_rooms.insert({'id' : 2, 'name': 'sauna',     'balance': 0,  'donate_amount' : 2})

#create members as example
db_members.purge() #start with fresh database
db_members.insert({'tag' : 123, 'name' : 'Peter' , 'balance' : 120})
db_members.insert({'tag' : 456, 'name' : 'Paul'  , 'balance' : 19})
db_members.insert({'tag' : 789, 'name' : 'Mary'  , 'balance' : 55})


# do loop
client.loop_forever()
