workspace "Staff"
  architecture "x64"
  configurations { "Debug", "Release" }
  startproject "ClefApp"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

include "ClefExternal.lua"
include "ClefApp"
