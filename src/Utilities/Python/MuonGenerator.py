#!/usr/bin/env python3
###############################################
# 						Muon Flux Generator
# Authors: M. Hampel, A. Taboada
# Date: March 2023
###############################################

import matplotlib.pyplot as plt
import numpy as np
import random
import argparse
from shapely.geometry import Polygon, Point
from scipy.spatial.transform import Rotation as R
# tqdm displays bar progress in loops
from tqdm import tqdm

PDF = np.array([5.680e-03, 7.290e-03, 9.340e-03, 1.131e-02, 1.413e-02, 1.722e-02,
								2.156e-02, 2.465e-02, 2.978e-02, 3.595e-02, 3.962e-02, 4.390e-02,
								4.995e-02, 5.468e-02, 5.791e-02, 5.973e-02, 6.099e-02, 5.971e-02,
								5.674e-02, 5.436e-02, 4.908e-02, 4.408e-02, 3.819e-02, 3.153e-02,
								2.647e-02, 2.174e-02, 1.741e-02, 1.390e-02, 1.043e-02, 7.890e-03,
								6.700e-03, 4.780e-03, 3.640e-03, 2.560e-03, 2.160e-03, 1.450e-03,
								8.600e-04, 6.500e-04, 5.200e-04, 4.500e-04, 2.500e-04, 2.200e-04,
								2.500e-04, 1.000e-04, 8.000e-05, 5.000e-05, 1.000e-05, 1.000e-05,
								3.000e-05, 1.000e-05])
# in log10(GeV)
logP = np.array([-0.96009144, -0.88036846, -0.80064549, -0.72092251, -0.64119954,
								-0.56147656, -0.48175359, -0.40203061, -0.32230764, -0.24258466,
								-0.16286169, -0.08313871, -0.00341574,  0.07630724,  0.15603021,
								0.23575319,  0.31547616,  0.39519914,  0.47492211,  0.55464509,
								0.63436806,  0.71409104,  0.79381401,  0.87353699,  0.95325996,
								1.03298294,  1.11270591,  1.19242889,  1.27215186,  1.35187484,
								1.43159781,  1.51132079,  1.59104376,  1.67076674,  1.75048971,
								1.83021269,  1.90993566,  1.98965864,  2.06938161,  2.14910459,
								2.22882756,  2.30855054,  2.38827351,  2.46799649,  2.54771946,
								2.62744244,  2.70716541,  2.78688839,  2.86661136,  2.94633434])

def cart2pol(x, y, z):
		# rho = np.arctan(np.sqrt(x**2 + y**2)/z)
		rho = np.arctan2(np.sqrt(x ** 2 + y ** 2), z)
		phi = np.arctan2(y, x)
		r = np.sqrt(x ** 2 + y ** 2 + z ** 2)
		return (np.array([rho, phi, r]))


def pol2cart(rho, phi, r):
		x = r * np.sin(rho) * np.cos(phi)
		y = r * np.sin(rho) * np.sin(phi)
		z = r * np.cos(rho)
		return (np.array([x, y, z]))




def GenerateAngle(theta_min, theta_max):
		tm = theta_min+(theta_max-theta_min) * np.random.uniform(0., 1.)
		tm1 = 2*tm/(np.pi)
		return np.arccos((1-tm1)**(1/3))

def cos2(x):
		#return (((np.cos(x)) ** 2)*np.sin(x) * np.cos(x))
		return (((np.cos(x)) ** 2)*np.sin(x))


def GenerateSample(f, bounds, N=50000):
		# generic way of sampling following a distribution given by f
		xmin = bounds[0]
		xmax = bounds[1]
		xtest = np.linspace(xmin, xmax, N)
		fmax = max(f(xtest))

		Nmax = N
		n = 0
		counts = []

		# create sample of given a distribution
		while (n < Nmax):
				r1 = np.random.uniform(0., 1.)
				r2 = np.random.uniform(0., 1.)

				x = r1 * (xmax - xmin) + xmin
				y = r2 * fmax

				if (f(x) > y):
						counts.append(x)
						n += 1

		return counts


def random_points_within(poly, num_points):
		min_x, min_y, max_x, max_y = poly.bounds

		points = []

		while len(points) < num_points:
				random_point = Point([random.uniform(min_x, max_x), random.uniform(min_y, max_y)])
				if (random_point.within(poly)):
						points.append(random_point)

		return points


def roto_trans(punto, theta, phi, x, y, z):
		rotation_axis = np.array([0, 1, 0])
		rotation_vector = theta * rotation_axis
		rotation = R.from_rotvec(rotation_vector)
		rot_aux = rotation.apply(punto)

		rotation_axis = np.array([0, 0, 1])
		rotation_vector = phi * rotation_axis
		rotation = R.from_rotvec(rotation_vector)
		rot_aux = rotation.apply(rot_aux)

		# rot_aux = cart2pol(punto[0], punto[1], punto[2])
		# rot_aux = pol2cart(rot_aux[0] + theta, rot_aux[1] + phi, rot_aux[2])
		rot_aux = rot_aux + [x, y, z]
		return rot_aux


class MuonGenerator:
		"""MuonGenerator
		Parameters and default values
				Nm=167  Number of muon per m² per second
				sc_x=0  X Origin of the sphere in m
				sc_y=0  Y Origin of the sphere in m
				sc_z=0  Z Origin of the sphere in  m
				sc_r=200    Radios of the sphere in m
				h1=2    Height of the Detector from its centre in m
				a=0.5   Separation between planes in m
				width=0.5   Detector plane width in m
				length=0.5   Detector plane length in m
				elevation=0*np.pi/180   Elevation angle of the Detector with respect to the Z axes in radians
				azimuth=0*np.pi/180    Azimuth angle of the Detector with respect to the x axes in radians
		"""

		def __init__(self,  *args, **kwargs):

				# Defino el rate de muones por m^2

				if kwargs.get('Nm') != None:
						self.Nm = kwargs.get('Nm')
				else:
						self.Nm = 167

				# Defino el centro de coordenadas de la esfera y el radio

				if kwargs.get('sc_x') != None:
						self.sc_x = kwargs.get('sc_x')
				else:
						self.sc_x = 0

				if kwargs.get('sc_y') != None:
						self.sc_y = kwargs.get('sc_y')
				else:
						self.sc_y = 0

				if kwargs.get('sc_z') != None:
						self.sc_z = kwargs.get('sc_z')
				else:
						self.sc_z = 0

				if kwargs.get('sc_r') != None:
						self.sc_r = kwargs.get('sc_r')
				else:
						self.sc_r = 200

				# Defino el detector top y bot

				if kwargs.get('h1') != None:
						self.h1 = kwargs.get('h1')
				else:
						self.h1 = 0.2

				if kwargs.get('a') != None:
						self.a = kwargs.get('a')
				else:
						self.a = 0.5

				if kwargs.get('width') != None:
						self.width = kwargs.get('width')
				else:
						self.width = 0.5

				if kwargs.get('length') != None:
						self.length = kwargs.get('length')
				else:
						self.length = 0.5

				# Define la rotación sobre el eje X

				if kwargs.get('elevation') != None:
						self.elevation = kwargs.get('elevation')
				else:
						self.elevation = 20 * np.pi / 180

				# Define la rotación sobre el eje Z

				if kwargs.get('azimuth') != None:
						self.azimuth = kwargs.get('azimuth')
				else:
						self.azimuth = 0 * np.pi / 180

				# Define intervalo para azimuth
				if kwargs.get('deltaPhi') != None:
						self.deltaPhi = kwargs.get('deltaPhi')
				else:
						self.deltaPhi = 180 * np.pi / 180

				self.p0_t = np.array([-self.width / 2, -self.length / 2, self.a])
				self.p1_t = np.array([-self.width / 2, self.length / 2, self.a])
				self.p2_t = np.array([self.width / 2, -self.length / 2, self.a])
				self.p3_t = np.array([self.width / 2, self.length / 2, self.a])

				self.p0_t = roto_trans(self.p0_t, self.elevation, self.azimuth, 0, 0, self.h1)
				self.p1_t = roto_trans(self.p1_t, self.elevation, self.azimuth, 0, 0, self.h1)
				self.p2_t = roto_trans(self.p2_t, self.elevation, self.azimuth, 0, 0, self.h1)
				self.p3_t = roto_trans(self.p3_t, self.elevation, self.azimuth, 0, 0, self.h1)

				self.p01_t = self.p1_t - self.p0_t
				self.p02_t = self.p2_t - self.p0_t

				self.p0_b = np.array([-self.width / 2, -self.length / 2, -self.a])
				self.p1_b = np.array([-self.width / 2, self.length / 2, -self.a])
				self.p2_b = np.array([self.width / 2, -self.length / 2, -self.a])
				self.p3_b = np.array([self.width / 2, self.length / 2, -self.a])

				self.p0_b = roto_trans(self.p0_b, self.elevation, self.azimuth, 0, 0, self.h1)
				self.p1_b = roto_trans(self.p1_b, self.elevation, self.azimuth, 0, 0, self.h1)
				self.p2_b = roto_trans(self.p2_b, self.elevation, self.azimuth, 0, 0, self.h1)
				self.p3_b = roto_trans(self.p3_b, self.elevation, self.azimuth, 0, 0, self.h1)

				self.p01_b = self.p1_b - self.p0_b
				self.p02_b = self.p2_b - self.p0_b

				# Determino la sombra

				p0_o = np.array([-1, -1, 0])
				p1_o = np.array([-1, 1, 0])
				p2_o = np.array([1, -1, 0])
				p01_o = p1_o - p0_o
				p02_o = p2_o - p0_o

				lab_1 = self.p3_t - self.p0_b
				#t_o = np.inner(np.cross(p01_o, p02_o), (self.p0_b - p0_o)) / np.inner(-lab_1, np.cross(p01_o, p02_o))
				u_o = np.inner(np.cross(p02_o, -lab_1), (self.p0_b - p0_o)) / np.inner(-lab_1, np.cross(p01_o, p02_o))
				v_o = np.inner(np.cross(-lab_1, p01_o), (self.p0_b - p0_o)) / np.inner(-lab_1, np.cross(p01_o, p02_o))
				P0 = p0_o + p01_o * u_o + p02_o * v_o

				lab_1 = self.p2_t - self.p1_b
				#t_o = np.inner(np.cross(p01_o, p02_o), (self.p1_b - p0_o)) / np.inner(-lab_1, np.cross(p01_o, p02_o))
				u_o = np.inner(np.cross(p02_o, -lab_1), (self.p1_b - p0_o)) / np.inner(-lab_1, np.cross(p01_o, p02_o))
				v_o = np.inner(np.cross(-lab_1, p01_o), (self.p1_b - p0_o)) / np.inner(-lab_1, np.cross(p01_o, p02_o))
				P1 = p0_o + p01_o * u_o + p02_o * v_o

				lab_1 = self.p0_t - self.p3_b
				#t_o = np.inner(np.cross(p01_o, p02_o), (self.p3_b - p0_o)) / np.inner(-lab_1, np.cross(p01_o, p02_o))
				u_o = np.inner(np.cross(p02_o, -lab_1), (self.p3_b - p0_o)) / np.inner(-lab_1, np.cross(p01_o, p02_o))
				v_o = np.inner(np.cross(-lab_1, p01_o), (self.p3_b - p0_o)) / np.inner(-lab_1, np.cross(p01_o, p02_o))
				P3 = p0_o + p01_o * u_o + p02_o * v_o

				lab_1 = self.p1_t - self.p2_b
				#t_o = np.inner(np.cross(p01_o, p02_o), (self.p2_b - p0_o)) / np.inner(-lab_1, np.cross(p01_o, p02_o))
				u_o = np.inner(np.cross(p02_o, -lab_1), (self.p2_b - p0_o)) / np.inner(-lab_1, np.cross(p01_o, p02_o))
				v_o = np.inner(np.cross(-lab_1, p01_o), (self.p2_b - p0_o)) / np.inner(-lab_1, np.cross(p01_o, p02_o))
				P2 = p0_o + p01_o * u_o + p02_o * v_o

				# Genero el polígono que es la proyección del detector en el suelo.
				self.poly = Polygon([P0[0:2], P1[0:2], P3[0:2], P2[0:2]])

				self.area = self.poly.area
				#print(self.area)

				self.impacts = []
				self.momentum = []
				self.origin = []

		def DetectorUpdate(self,  *args, **kwargs):
				# Defino el rate de muones por m^2

				if kwargs.get('Nm') != None:
						self.Nm = kwargs.get('Nm')

				# Defino el centro de coordenadas de la esfera y el radio

				if kwargs.get('sc_x') != None:
						self.sc_x = kwargs.get('sc_x')

				if kwargs.get('sc_y') != None:
						self.sc_y = kwargs.get('sc_y')

				if kwargs.get('sc_z') != None:
						self.sc_z = kwargs.get('sc_z')

				if kwargs.get('sc_r') != None:
						self.sc_r = kwargs.get('sc_r')

				# Defino el detector top y bot

				if kwargs.get('h1') != None:
						self.h1 = kwargs.get('h1')

				if kwargs.get('a') != None:
						self.a = kwargs.get('a')

				if kwargs.get('width') != None:
						self.width = kwargs.get('width')

				if kwargs.get('length') != None:
						self.length = kwargs.get('length')

				# Define la rotación sobre el eje X

				if kwargs.get('elevation') != None:
						self.elevation = kwargs.get('elevation')

				# Define la rotación sobre el eje Z

				if kwargs.get('azimuth') != None:
						self.azimuth = kwargs.get('azimuth')

				# Define intervalo para azimuth
				if kwargs.get('deltaPhi') != None:
						self.deltaPhi = kwargs.get('deltaPhi')

				self.p0_t = np.array([-self.width / 2, -self.length / 2, self.a])
				self.p1_t = np.array([-self.width / 2, self.length / 2, self.a])
				self.p2_t = np.array([self.width / 2, -self.length / 2, self.a])
				self.p3_t = np.array([self.width / 2, self.length / 2, self.a])

				self.p0_t = roto_trans(self.p0_t, self.elevation, self.azimuth, 0, 0, self.h1)
				self.p1_t = roto_trans(self.p1_t, self.elevation, self.azimuth, 0, 0, self.h1)
				self.p2_t = roto_trans(self.p2_t, self.elevation, self.azimuth, 0, 0, self.h1)
				self.p3_t = roto_trans(self.p3_t, self.elevation, self.azimuth, 0, 0, self.h1)

				self.p01_t = self.p1_t - self.p0_t
				self.p02_t = self.p2_t - self.p0_t

				self.p0_b = np.array([-self.width / 2, -self.length / 2, -self.a])
				self.p1_b = np.array([-self.width / 2, self.length / 2, -self.a])
				self.p2_b = np.array([self.width / 2, -self.length / 2, -self.a])
				self.p3_b = np.array([self.width / 2, self.length / 2, -self.a])

				self.p0_b = roto_trans(self.p0_b, self.elevation, self.azimuth, 0, 0, self.h1)
				self.p1_b = roto_trans(self.p1_b, self.elevation, self.azimuth, 0, 0, self.h1)
				self.p2_b = roto_trans(self.p2_b, self.elevation, self.azimuth, 0, 0, self.h1)
				self.p3_b = roto_trans(self.p3_b, self.elevation, self.azimuth, 0, 0, self.h1)

				self.p01_b = self.p1_b - self.p0_b
				self.p02_b = self.p2_b - self.p0_b

				# Determino la sombra

				p0_o = np.array([-1, -1, 0])
				p1_o = np.array([-1, 1, 0])
				p2_o = np.array([1, -1, 0])
				p01_o = p1_o - p0_o
				p02_o = p2_o - p0_o

				lab_1 = self.p3_t - self.p0_b
				#t_o = np.inner(np.cross(p01_o, p02_o), (self.p0_b - p0_o)) / np.inner(-lab_1, np.cross(p01_o, p02_o))
				u_o = np.inner(np.cross(p02_o, -lab_1), (self.p0_b - p0_o)) / np.inner(-lab_1, np.cross(p01_o, p02_o))
				v_o = np.inner(np.cross(-lab_1, p01_o), (self.p0_b - p0_o)) / np.inner(-lab_1, np.cross(p01_o, p02_o))
				P0 = p0_o + p01_o * u_o + p02_o * v_o

				lab_1 = self.p2_t - self.p1_b
				#t_o = np.inner(np.cross(p01_o, p02_o), (self.p1_b - p0_o)) / np.inner(-lab_1, np.cross(p01_o, p02_o))
				u_o = np.inner(np.cross(p02_o, -lab_1), (self.p1_b - p0_o)) / np.inner(-lab_1, np.cross(p01_o, p02_o))
				v_o = np.inner(np.cross(-lab_1, p01_o), (self.p1_b - p0_o)) / np.inner(-lab_1, np.cross(p01_o, p02_o))
				P1 = p0_o + p01_o * u_o + p02_o * v_o

				lab_1 = self.p0_t - self.p3_b
				#t_o = np.inner(np.cross(p01_o, p02_o), (self.p3_b - p0_o)) / np.inner(-lab_1, np.cross(p01_o, p02_o))
				u_o = np.inner(np.cross(p02_o, -lab_1), (self.p3_b - p0_o)) / np.inner(-lab_1, np.cross(p01_o, p02_o))
				v_o = np.inner(np.cross(-lab_1, p01_o), (self.p3_b - p0_o)) / np.inner(-lab_1, np.cross(p01_o, p02_o))
				P3 = p0_o + p01_o * u_o + p02_o * v_o

				lab_1 = self.p1_t - self.p2_b
				#t_o = np.inner(np.cross(p01_o, p02_o), (self.p2_b - p0_o)) / np.inner(-lab_1, np.cross(p01_o, p02_o))
				u_o = np.inner(np.cross(p02_o, -lab_1), (self.p2_b - p0_o)) / np.inner(-lab_1, np.cross(p01_o, p02_o))
				v_o = np.inner(np.cross(-lab_1, p01_o), (self.p2_b - p0_o)) / np.inner(-lab_1, np.cross(p01_o, p02_o))
				P2 = p0_o + p01_o * u_o + p02_o * v_o

				# Genero el polígono que es la proyección del detector en el suelo.
				self.poly = Polygon([P0[0:2], P1[0:2], P3[0:2], P2[0:2]])

				self.area = self.poly.area
				#print(self.area)

				self.impacts = []
				self.momentum = []
				self.origin = []
				self.theta = []
				self.phi = []

		def MuonGenerate(self, *args, **kwargs):
				if kwargs.get('N') != None:
						self.Nmu_requested = kwargs.get('N')
				else:
						self.Nmu_requested = 1

				self.impacts = []
				self.momentum = []
				self.origin = []
				self.theta = []
				self.phi = []


				Ntot = 0
				Nmu = 0
				pbar = tqdm(total=self.Nmu_requested)
				while (Nmu < self.Nmu_requested):
						
						theta = GenerateAngle(0, np.pi/2)
						self.theta.append(theta)
						# generate random momentum value given its PDF
						ptot = np.random.choice(logP, p=PDF)

						#theta = GenerateSample(cos2, [0, np.pi / 2], 1)
						phi = np.random.uniform(low=self.azimuth-self.deltaPhi, high=self.azimuth+self.deltaPhi, size=1)
						self.phi.append(phi)
						#aux = pol2cart(theta[0], phi[0], 1)
						aux = pol2cart(theta, phi[0], 1)

						points = random_points_within(self.poly, 1)
						la = [np.array((geom.xy[0][0], geom.xy[1][0], 0)) for geom in points][0]
						lb = [np.array((geom.xy[0][0] + aux[0], geom.xy[1][0] + aux[1], aux[2])) for geom in points][0]

						lab = lb - la

						t_t = np.inner(np.cross(self.p01_t, self.p02_t), (la - self.p0_t)) / np.inner(-lab, np.cross(self.p01_t, self.p02_t))
						u_t = np.inner(np.cross(self.p02_t, -lab), (la - self.p0_t)) / np.inner(-lab, np.cross(self.p01_t, self.p02_t))
						v_t = np.inner(np.cross(-lab, self.p01_t), (la - self.p0_t)) / np.inner(-lab, np.cross(self.p01_t, self.p02_t))

						t_b = np.inner(np.cross(self.p01_b, self.p02_b), (la - self.p0_b)) / np.inner(-lab, np.cross(self.p01_b, self.p02_b))
						u_b = np.inner(np.cross(self.p02_b, -lab), (la - self.p0_b)) / np.inner(-lab, np.cross(self.p01_b, self.p02_b))
						v_b = np.inner(np.cross(-lab, self.p01_b), (la - self.p0_b)) / np.inner(-lab, np.cross(self.p01_b, self.p02_b))

						Ntot += 1

						if 0 <= u_t <= 1 and 0 <= v_t <= 1 and 0 <= u_b <= 1 and 0 <= v_b <= 1:
								Nmu += 1
								self.impacts.append(la)

								a = lab[0] ** 2 + lab[1] ** 2 + lab[2] ** 2
								b = 2 * (lab[0] * (la[0] - self.sc_x) + lab[1] * (la[1] - self.sc_y) + lab[2] * (la[2] - self.sc_z))
								c = self.sc_x ** 2 + self.sc_y ** 2 + self.sc_z ** 2
								c += la[0] ** 2 + la[1] ** 2 + la[2] ** 2
								c -= 2 * (self.sc_x * la[0] + self.sc_y * la[1] + self.sc_z * la[2])
								c -= self.sc_r ** 2
								bb4ac = b * b - 4 * a * c

								mu1 = (-b + np.sqrt(bb4ac)) / (2 * a)
								#mu2 = (-b - np.sqrt(bb4ac)) / (2 * a)

								self.origin.append(la + mu1 * lab)
								self.momentum.append(10**ptot * (-lab / np.sqrt(lab[0] ** 2 + lab[1] ** 2 + lab[2] ** 2)))
								
								pbar.update(1)
				pbar.close()
				return(Ntot)

def main():
	# definition of main function
	parser = argparse.ArgumentParser(description='Muon Flux Generator. Authors: M. Hampel, A. Taboada, A. Sedoski')
	parser.add_argument('outfile', type=argparse.FileType('w'), help='File containing the generated muon flux')
	parser.add_argument('--num', type=int, help='Number of muons to be generated', default=1)
	parser.add_argument('--radius', type=float, help='Radius of the sphere', default=200.)
	parser.add_argument('--dheight', type=float, help='Height of the detector (m)', default=1.4)
	parser.add_argument('--dplane', type=float, help='Distance between detector planes (m)', default=1.)
	parser.add_argument('--width', type=float, help='Detector width (m)', default=0.5)
	parser.add_argument('--length', type=float, help='Detector Length (m)', default=0.5)
	parser.add_argument('--elevation', type=float, help='Elevation angle (deg)', default=0.)
	parser.add_argument('--azimuth', type=float, help='Azimuth angle (deg)', default=90.)
	parser.add_argument('--dphi', type=float, help='Restrict simulation up to (deg)', default=180.)
	parser.add_argument('--plot', type=bool, help='Plot results', default=False)

	args = parser.parse_args()

	# Generate Muons
	muon = MuonGenerator(Nm=167, sc_x=0, sc_y=0, sc_z=0, sc_r=args.radius, h1=args.dheight, a=args.dplane, width=args.width, length=args.length, elevation=args.elevation*np.pi/180, azimuth=args.azimuth*np.pi/180, deltaPhi=args.dphi*np.pi/180)

	Ntot = muon.MuonGenerate(N=args.num) * (360) / (args.dphi*2)

	print(Ntot)

	# write sample flux into file
	
	entries = args.num
	lines = []
	# loop over energy bins
	for i in range(entries):

			line = []
			
			# add CORSIKA ID (mu = 6)
			line.append(6)
			# add momentum components in [GeV]
			line.append(muon.momentum[i][0])
			line.append(muon.momentum[i][1])
			line.append(-muon.momentum[i][2])
			# add position coordinates in [cm]
			line.append(muon.origin[i][0] * 1e2)
			line.append(muon.origin[i][1] * 1e2)
			line.append(muon.origin[i][2] * 1e2 - 125 * 1e2)

			# set remaining column values to zero
			newline = line + [0, 0, 0, 0, 0]

			lines.append(newline)

	for line in lines:
			
			args.outfile.write("%i\t" %line[0])
			for d in line[1:7]:
					args.outfile.write("{:e}\t".format(d))
			for d in line[7:9]:
					args.outfile.write("%i\t" %d)
					
			args.outfile.write("%e\t %.3f\t %.3f" %(line[9], line[10], line[11]))


			args.outfile.write("\n")

	# closing output file
	args.outfile.close()
	print(args.num / Ntot * muon.Nm * muon.area)

	##############################################################################
	# Plot
	##############################################################################
	if args.plot:
		# 3D plot of muon positions on the sky
		# plot maximum of 100 muons (just for visualization)
		maxNumTraj = min(100, args.num)
		fig1 = plt.figure()
		ax1 = fig1.add_subplot(projection='3d')
		ax1.quiver(np.array(muon.origin)[:maxNumTraj, 0], np.array(muon.origin)[:maxNumTraj, 1], np.array(muon.origin)[:maxNumTraj, 2], 
												np.array(muon.momentum)[:maxNumTraj, 0], np.array(muon.momentum)[:maxNumTraj, 1], np.array(muon.momentum)[:maxNumTraj, 2], 
												color='k', alpha=0.2)

		ax1.set_xlim([-muon.sc_r * 1.1, muon.sc_r * 1.1])
		ax1.set_ylim([-muon.sc_r * 1.1, muon.sc_r * 1.1])
		ax1.set_zlim([0, muon.sc_r * 1.1])
		ax1.set_xlabel(r'X / m')
		ax1.set_ylabel(r'Y / m')
		ax1.set_zlabel(r'Z / m')
		
		bins = int(np.sqrt(args.num))
		# plot elevation and azimuth distributions		
		fig5 = plt.figure()
		plt.hist(np.degrees(np.array(muon.theta)), bins=bins, lw=2, histtype='step', color='k')
		plt.xlabel(r'Elvetation / deg')
		plt.ylabel(r'Counts')

		fig6 = plt.figure()
		plt.hist(np.degrees(np.array(muon.phi)), bins=bins, lw=2, histtype='step', color='k')
		plt.xlabel(r'Azimuth / deg')
		plt.ylabel(r'Counts')
		
		px = np.array(muon.momentum)[:, 0]
		py = np.array(muon.momentum)[:, 1]
		pz = np.array(muon.momentum)[:, 2]
		p = np.sqrt(px*px + py*py + pz*pz)

		phi = np.arctan2(py, pz)
		theta = np.arccos(-pz/p)

		# plot distribution of muon momentum
		fig2 = plt.figure()
		plt.hist(np.log10(p), bins=bins, histtype='step', lw=2, color='k')
		plt.yscale('log')
		plt.xlabel(r'log10(p / GeV)')
		plt.ylabel(r'Counts')

		# plot zenith and azimuth distributions
		fig3 = plt.figure()
		plt.hist(np.degrees(theta), bins=bins, histtype='step', lw=2, color='k')
		plt.xlabel(r'Muon Zenith / deg')
		plt.ylabel(r'Counts')

		fig4 = plt.figure()
		plt.hist(np.degrees(phi), bins=bins, histtype='step', lw=2, color='k')
		plt.xlabel(r'Muon Azimuth / deg')
		plt.ylabel(r'Counts')

		plt.show()


if __name__ == '__main__':
	main()