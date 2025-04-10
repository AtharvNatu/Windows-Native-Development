import win32com.client

if __name__ == "__main__":
    sd = win32com.client.Dispatch("StableDiffusion.Server")

    prompt = "Tigers on a beach"
    output_path = "F:\\Windows-Native-Development\\03-Projects\\Image-Editor\\02-Integration\\Version-03\\Test\\Clients\\Python\\Output.png"

    try:
        print(sd.GenerateImage(prompt, output_path))
    except Exception as e:
        print(e)

