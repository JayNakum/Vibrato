workspace "Staff"
  architecture "x64"
  configurations { "Debug", "Release" }
  startproject "Vibrato"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

include "ClefExternal.lua"
include "Vibrato"
