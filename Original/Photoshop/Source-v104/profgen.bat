COPY %1.8bf save
PREP /OM %1.8bf
COPY %1._bf %1.8bf
PROFILE /I %1 /O %1 %2 %3 %4 %5 %6
COPY save %1.8bf
PREP /M %1
PLIST %1
