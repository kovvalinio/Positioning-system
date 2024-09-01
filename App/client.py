import socket
import os


def parse_and_save_to_json(data):
    try:
        la_fl = float(data[2:11])
        lo_fl = float(data[14:24])
        lati = str(round(float(data[0:2]) + la_fl/60.0000,6))
        longi = str(round(float(data[12:14])+ lo_fl/60.0000,6))
        file = open("coordinates.json",'w')
        file.write("{\"id\":25544,\"latitude\":" + lati + ",\"longitude\":"+longi+"}")
        file.close()
    except:
        print("Invalid format or the problem with writing to json!")

def get_req_cords():
    try:
        file = open("/home/michal/Downloads/requested_cords.json",'r')
        data = file.read()
        lat = round(float(data[data.find("{lat: ")+6:data.find(",")]),6)
        long = round(float(data[data.find(", lng:")+6:data.find("}")]),6)
        print(long)
        os.remove("/home/michal/Downloads/requested_cords.json")
        return lat, long
    except:
        return None, None



host = ''
port =                    # The same port as used by the server
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((host, port))
while(True):
    data = s.recv(1024)
    print(data)
    parse_and_save_to_json(data)
    lat, long = get_req_cords()
    if lat != None and long !=None:
        s.send(("   REQUESTED CORDS: {"+str(lat)+","+str(long)+"} ").encode())
s.close()
