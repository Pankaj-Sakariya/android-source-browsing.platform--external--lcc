#!/usr/bin/python
#
# Test C code on Android

import os
import subprocess
import sys

testFiles = [
  "8q",
  "array",
  "fields",
  "hello",
  "incr",
  "init",
  "limits",
  "sort",
  "struct",
  "switch",
  "testmod"
]

slow = [
  "nsievebits"
]

problems = [
  "cf", # floating point
  "cq", # floating point
  "cvt", # floating point
  "front", # This is a test of the front end, it's supposed to fail in a variety of ways.
  "paranoia", # inline asm
  "spill", # floating point
  "stdarg", # preprocessor issues
  "wf1", #requires input on stdin
  "yacc", # requires a working stdin
  ""
]

onDeviceTestDir = "/system/bin/"

def adbGetState():
  return subprocess.Popen(["adb", "get-state"], stdout=subprocess.PIPE).communicate()[0].strip()

def adbRemount():
  return subprocess.Popen(["adb", "remount"], stdout=subprocess.PIPE).communicate()[0].strip()

def compile(f):
  srcPath = f + ".c"
  outPath = f
  return subprocess.Popen(["lcc", f + ".c", "-o", f],
    stdout=subprocess.PIPE).communicate()[0].strip()

def transferToAndroid(f):
  transferText = subprocess.Popen(["adb", "push", f, onDeviceTestDir + f],
    stdout=subprocess.PIPE).communicate()[0].strip()
    
def normalizeOutput(str):
  return str.replace("\r","")

def run(f):
  return normalizeOutput(subprocess.Popen(["adb", "shell", onDeviceTestDir + f],
    stdout=subprocess.PIPE).communicate()[0])
    
def timeRun(f):
  return normalizeOutput(subprocess.Popen(["time", "adb", "shell", onDeviceTestDir + f],
    stdout=subprocess.PIPE).communicate()[0])

def compareResult(f, output):
  expected = ""
  expectedPath = f + ".expected"
  if os.path.exists(expectedPath):
    expectedFile = open(f + ".expected", "r")
    expected = expectedFile.read()
    expectedFile.close()
    expected = expected.strip()
  output = output.strip()
  if expected == output:
     print timeRun(f)
     return True
  print "results differed. Expected:"
  print expected
  print "Actual:"
  print output
  return False

def test(testFile):
  print "testing: " + testFile
  compile(testFile)
  transferToAndroid(testFile)
  os.remove(testFile)
  output = run(testFile)
  compareResult(testFile, output)

def doTests():
  for testFile in testFiles:
    test(testFile)

def checkForDevice():
  if adbGetState() != "device":
      print "We do not appear to be connected to a device."
      sys.exit(1)

def main():
  checkForDevice()
  print adbRemount()
  doTests()

main()