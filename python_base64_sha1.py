# coding:utf-8

import hashlib,base64,sys

sha1=hashlib.sha1()

sha1.update(sys.argv[1])

# print 'sha1:',
# for i in xrange(len(sha1.digest())):print sha1.digest()[i].encode('hex'),
# print ''

SWSA=base64.b64encode(sha1.digest())

print '原先(\'scott\' sha1 then base64): '+SWSA
