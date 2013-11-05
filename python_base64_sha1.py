# coding:utf-8

import hashlib,base64

sha1=hashlib.sha1()

sha1.update("scott")

for i in xrange(len(sha1.digest())):
	print sha1.digest()[i].encode('hex'),
print

print sha1.digest()

SWSA=base64.b64encode(sha1.digest())

print '單純看 base64(scott to): '+base64.b64encode('scott')

print SWSA
