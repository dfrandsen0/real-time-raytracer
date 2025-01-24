all: raytracer safety

raytracer: ray.c
	gcc -o raytracer ray.c helperCode/other.c helperCode/loop.c helperCode/movement.c helperCode/setup.c helperCode/tracerMath.c -lm
safety: safety.c
	gcc -o safety safety.c

clean:
	rm -f safety
	rm -f raytracer
