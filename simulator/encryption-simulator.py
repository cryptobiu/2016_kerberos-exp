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
    print " - new request to be encrypted."
    print "   Dump:   (data)"
    hexdump.hexdump(body[:16])
    print "   Dump:   (nonce+counter)"
    hexdump.hexdump(body[16:])
    print "   we are going to reply using the key: password"
    response = encrypt(body)
    print " Reply dump:"
    hexdump.hexdump(response)

    ch.basic_publish(exchange='',
            routing_key=props.reply_to,
            properties=pika.BasicProperties(correlation_id = \
                    props.correlation_id),
            body="teste")
    ch.basic_ack(delivery_tag = method.delivery_tag)


def encrypt(data):
    key = "bc155de16c053f81a4dd3f907c58f88b"  # Should be password, but it is not :(
    key = "286755fad04869ca523320acce0dc6a4"
    nonce = data[:8]
    data = data[16:]
    ctr_e = Counter.new(64, initial_value=0, prefix=nonce)

    enc = AES.new(key, mode=AES.MODE_CTR, counter=ctr_e)
    edata = enc.encrypt(data)

    return edata

if __name__ == "__main__":
        main()

