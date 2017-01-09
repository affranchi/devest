#!/usr/bin/python3

import memcache

mc = memcache.Client(['127.0.0.1:11211'], debug = 0)

mc.set("myKeyTest","This key is mine")
value = mc.get("myKeyTest")
mc.delete("myKeyTest")
print(value)

