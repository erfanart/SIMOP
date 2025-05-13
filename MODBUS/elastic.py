from elasticsearch import Elasticsearch

# Replace with your actual API key
api_key = "NHp1akxwSUJFYXFLa1JaQ3hpQkU6OTc1b2dfcWdSRC0yRDBldU9oMmtrUQ=="  # Example: "BASE64_ENCODED_API_KEY"

# Connect to the Elasticsearch cluster using the API key
es = Elasticsearch(
    "https://10.130.3.200:9200",
    api_key=api_key,
    verify_certs=False
)

# Define the document you want to send
message = {
    "message": "This is a test message",  # Replace this with your message content
    "tag": "looloo"
}

# Send the message to the specified index
response = es.index(index="gojeh", body=message)

# Print the response
print(response)
