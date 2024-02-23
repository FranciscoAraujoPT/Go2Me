import carla
import pygame
from pygame.locals import *

def main():
    try:
        # Connect to the Carla server
        client = carla.Client('localhost', 2000)
        client.set_timeout(2.0)

        # Load the map
        world = client.load_world('Town04')

        # Get the blueprint library
        blueprint_library = world.get_blueprint_library()

        # Choose a vehicle blueprint
        vehicle_bp = blueprint_library.filter('vehicle.*')[0]

        # Choose a spawn point
        spawn_point = carla.Transform(carla.Location(x=230, y=150, z=2), carla.Rotation(yaw=180))

        # Spawn the vehicle
        vehicle = world.spawn_actor(vehicle_bp, spawn_point)

        # Set up pygame for user input
        pygame.init()
        screen = pygame.display.set_mode((400, 300))
        pygame.display.set_caption('Carla Control')
        clock = pygame.time.Clock()

        # Control parameters
        throttle = 0.0
        steer = 0.0
        brake = 0.0

        running = True
        while running:
            for event in pygame.event.get():
                if event.type == QUIT:
                    running = False
                elif event.type == KEYDOWN:
                    if event.key == K_UP:
                        throttle = 1.0
                    elif event.key == K_DOWN:
                        brake = 1.0
                    elif event.key == K_LEFT:
                        steer = -1.0
                    elif event.key == K_RIGHT:
                        steer = 1.0
                elif event.type == KEYUP:
                    if event.key == K_UP:
                        throttle = 0.0
                    elif event.key == K_DOWN:
                        brake = 0.0
                    elif event.key == K_LEFT or event.key == K_RIGHT:
                        steer = 0.0

            # Apply control to the vehicle
            vehicle.apply_control(carla.VehicleControl(throttle=throttle, steer=steer, brake=brake))

            # Update pygame display
            pygame.display.flip()
            clock.tick(60)

    finally:
        # Destroy actors
        vehicle.destroy()
        pygame.quit()

if __name__ == '__main__':
    main()
