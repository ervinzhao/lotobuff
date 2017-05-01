#!/usr/bin/lua

local lotobuf = require "lotobuf"
local cjson = require "cjson"

local loto = lotobuf.new()

--loto:load('/home/zhaohongjian/workspace/protobuf-3.3.0/examples/addressbook.proto')
--loto:load('/home/zhaohongjian/workspace/lotobuf/examples/addressbook.proto')
local ret = loto:map('', './examples/')
print(ret)
loto:load('addressbook.proto')


local datafile = io.open('./examples/output.data', 'r')
data = datafile:read('*a')

local result, err = loto:decode('AddressBook', data)
print(result)
print(err)

print(cjson.encode(result))
