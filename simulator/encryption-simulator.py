#!/usr/bin/env python

from Crypto.Cipher import AES
from Crypto.Util import Counter
import hexdump
import hashlib
import os
import pika
import math

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
    l = int(100)
    try:
        l = int(data[32])
    except:
        pass
    print "l: " + str(l)

    enc = ""
    c = Counter.new(64, initial_value=0, prefix=nonce)
    e = AES.new(key, AES.MODE_ECB)
    n_blocks = int(math.ceil(l*8/128.0))
    for n in xrange(n_blocks):
        enc = enc + e.encrypt(str(c()))

    print "Nonce: "
    hexdump.hexdump(nonce)
    print "Count: "
    hexdump.hexdump(count)
    print "Key: "
    hexdump.hexdump(key)
    print "Enc: "
    hexdump.hexdump(enc)

    return enc

if __name__ == "__main__":
        main()

