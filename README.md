# CSVIO

[![Codacy Badge](https://api.codacy.com/project/badge/Grade/62ec3639aa1448058ab9abecb42294f4)](https://app.codacy.com/app/mguid65/csvio?utm_source=github.com&utm_medium=referral&utm_content=mguid65/csvio&utm_campaign=Badge_Grade_Dashboard)

Experimental header-only CSV IO classes using some modern C++17 features

Support for RFC 4180

## Why

A quick search for C++ csv library yields many responses along the lines of:

`Write your own`

Even though CSV isn't very complicated and isn't fully standardized,
why should you *have* to write your own. Why waste your time writing some boilerplate code.


## Features
 *  Reading/Writing std::iostreams
 *  Alternate delimiters
 *  Custom Row Input Parsing Functions
 *  Custom Row Output Formatting Functions
 *  Custom Line Readers
 *  Custom Line Writers
 *  Custom Swappable Row Containers
 *  Custom Escape Utilities

## Work In Progress
 *  Header inference
 *  Specialization for using Associative Container types
