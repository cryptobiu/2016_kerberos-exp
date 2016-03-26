#!/usr/bin/env python

from Crypto.Cipher import AES
from Crypto.Util import Counter
import hexdump
import hashlib
import os
import pika

def main():
    connection = pika.BlockingConnection(pika.ConnectionParameters(
        host='localhost'))
    channel = connection.channel()
    channel.queue_declare(queue='rpc_queue')

    channel.basic_qos(prefetch_count=1)
    channel.basic_consume(on_request, queue='rpc_queue')

    print(" That is the `encryption' simulator. Waiting for requests...")
    channel.start_consuming()


def on_request(ch, method, props, body):
    response = encrypt(body)

    ch.basic_publish(exchange='',
            routing_key=props.reply_to,
            properties=pika.BasicProperties(correlation_id = \
                    props.correlation_id),
            body=response)
    ch.basic_ack(delivery_tag = method.delivery_tag)


def encrypt(data):
    # nounce + countr - 16 bytes 
    # key / user id   - 16 bytes 
    # data            - n bytes 
    nonce = data[:8]
    count = data[8:16]
    key = data[16:32]
    data = data[32:]

    ctr_e = Counter.new(64, initial_value=0, prefix=nonce)
    enc = AES.new(key, mode=AES.MODE_CTR, counter=ctr_e)
    edata = enc.encrypt(data)

    print "Nonce: "
    hexdump.hexdump(nonce)
    print "Count: "
    hexdump.hexdump(count)
    print "Key: "
    hexdump.hexdump(key)
    print "Bofore encryption:"
    hexdump.hexdump(data)
    print "After encryption:"
    hexdump.hexdump(edata)
    return edata
"""
    print " ---- Optimization ---- "
    data2 = []
    for i in data:
        data2.append(0x00)
    ctr_e2 = Counter.new(64, initial_value=0, prefix=nonce)
    enc2 = AES.new(key, mode=AES.MODE_CTR, counter=ctr_e2)
    edata2 = enc.encrypt(data)
    print "Encrypted data:"
    hexdump.hexdump(edata2)
"""

if __name__ == "__main__":
        main()

