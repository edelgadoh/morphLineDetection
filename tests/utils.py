#!/usr/bin/env python
import unittest
#import projeto

class UtilsTestCase(unittest.TestCase):

    def test_url_join_http(self):
        self.assertEqual("http://example.com/", "http://example.com/")