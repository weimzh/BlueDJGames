del makefile*
del object*
del *.rc
del *.a
rd build /s/q
if not exist "tmp" goto EndTmp
cd "tmp"
rd moc /s/q
rd ui /s/q
if not exist "obj" goto EndObj
cd "obj"
del a*
del b*
del c*
del d*
del e*
del f*
del g*
del h*
del i*
del j*
del k*
del l*
del m*
del n*
del o*
del p*
del r*
del s*
del t*
del u*
del v*
del w*
del x*
del y*
del z*
cd ..
:EndObj
cd ..
:EndTmp




