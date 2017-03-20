from setuptools import setup

req = ['nose','numpy',
       'glymur']

setup(name='andor_scmos',
      packages=['andor_scmos'],
	  description='Python scripts controlling plain C++. Fast and simple.',
	  url='https://github.com/scivision/andor-scmos-examples',
	  author='Michael Hirsch, Ph.D.',
      install_requires=req,
	  )

