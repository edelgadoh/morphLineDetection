import os.path
from setuptools import setup, find_packages
import sys

base_dir = os.path.dirname(os.path.abspath(__file__))

setup(
    name = "projeto",
    version = "0.1.1",
    description = "Edge detection over microscopic images",
    long_description="\n\n".join([
        open(os.path.join(base_dir, "README"), "r").read(),
        open(os.path.join(base_dir, "AUTHORS"), "r").read()
    ]),
    url = "http://www.vision.ime.usp.br/~edelgado/projeto.php/",
    author = "Edwin Delgado H",
    author_email = "edychrist@gmail.com",
    #packages = find_packages(),
    zip_safe = False,
    #install_requires = install_requires,
    test_suite = "tests.get_tests",
)