from EDU_LPC import EDU_LPC_SOCK, Functions
from sys import argv

def main():
    """
    Main program
    """
    # Check if EDU LPC IP was given
    if len(argv) < 2:
        print("Falta la direccion de IP del servidor")
        exit(1)
    # Store IP
    server_ip = argv[1]
    # Create an EDU LPC SOCK instance
    sock = EDU_LPC_SOCK(server_ip)
    

if __name__ == '__main__':
    main()