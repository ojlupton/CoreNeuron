## CoreNEURON Documentation

### Local build

It is recommended using a `virtualenv`, for example:

```
pip3 install virtualenv
python3 -m virtualenv venv
source venv/bin/activate
```

In order to build documentation locally, you need to pip install the [requirements](requirements.txt) :
```
pip3 install --user -r docs/requirements.txt --upgrade
```

Then in your CMake build folder:
```
make docs
```  
That will build everything in the `build/docs` folder and you can then open `index.html` locally.

When working locally on documentation, be aware of the following targets to speed up building process:

* `doxygen` - build the API documentation only
* `sphinx` - build Sphinx documentation

