
min = 500
max = 2400
angle = 90


scale = ((max-min)/180)

pw = (min + (angle*scale)/2)

print("scale = "+str((scale)))
print("PW for angle of "+str(angle)+" degrees is "+str(int(pw))+" uS." )

CW = int((max - min)/(scale))

print("Full CW = "+ str(CW))
