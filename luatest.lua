#!/usr/bin/lua

local lotobuf = require "lotobuf"
local cjson = require "cjson"

local loto = lotobuf.new()

local ret = loto:map('', './examples/')
local status, err = loto:load('addressbook.proto')
if not status then
    print(err)
end


local datafile = io.open('./examples/output.data', 'r')
data = datafile:read('*a')

local result, err = loto:decode('tutorial.AddressBook', data)
print(result)
print(err)

print(cjson.encode(result))

local data2 = loto:encode('tutorial.AddressBook', result)

local result2, err = loto:decode('tutorial.AddressBook', data2)
print(cjson.encode(result2))

