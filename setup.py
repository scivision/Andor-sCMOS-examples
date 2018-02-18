from setuptools import setup, find_packages

install_requires = ['nose','numpy','imageio',
       'glymur']

setup(name='andor_scmos',
      packages=find_packages(),
	  description='Python scripts controlling Andor plain C++. Fast and simple.',
      long_description=open('README.rst').read(),
	  url='https://github.com/scivision/andor-scmos-examples',
	  author='Michael Hirsch, Ph.D.',
      install_requires=install_requires,
      python_requires='>=3.5',
	  )

