#define NOISE01(p) ((1.0f+noise4(p,phase))*.5f)

float Noise::Turb(Point3 p) {
	float sum = 0.0f;
	float l,f = 1.0f;
	for (l = levels; l>=1.0f; l-=1.0f) {
		sum += (float)fabs(noise4(p*f,phase))/f;
		f *= 2.0f;
		}
	if (l>0.0f)
		sum += l*(float)fabs(noise4(p*f,phase))/f;
	return sum;
	}

inline float Noise::NoiseFunc(Point3 p) {
	float res;
	switch (noiseType) {
		case NOISE_TURB:
			res = Turb(p);
			break;
	
		case NOISE_REGULAR:
			res = NOISE01(p);
			break;

		case NOISE_FRACTAL:
			if (levels==1.0f) {
				res = NOISE01(p);
			} else {
				float sum = 0.0f;
				float l,f = 1.0f;			
				for (l = levels; l>=1.0f; l-=1.0f) {				
					sum += noise4(p*f,phase)/f;
					f *= 2.0f;
					}
				if (l>0.0f)				
					sum += l*noise4(p*f,phase)/f;				
				res = 0.5f*(sum+1.0f);
				}
			break;
		}

	if (low<high) {
		res = threshold(res,low,high);
		}
	if (res<0.0f) res = 0.0f;
	if (res>1.0f) res = 1.0f;
	return res;
	}
