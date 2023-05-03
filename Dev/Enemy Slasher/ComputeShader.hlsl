[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
}



//matrix gmtxGameObject : packoffset(b0);
//matrix gmtxGameObject : packoffset(b1);
//
//cbuffer ObjectInfo : register(b2)
//{
//    float3 object1_position;
//    float3 object1_scale;
//    float3 object2_position;
//    float3 object2_scale;
//};
//
//
//StructuredBuffer<VertexInput> inputBuffer : register(t0); // ���� ����
//RWStructuredBuffer<VertexOutput> outputBuffer : register(u0); // ���� ��ȯ �� ����
//StructuredBuffer<ObjectData> objectBuffer : register(t1); // ������Ʈ ���� ����

//[numthreads(64, 1, 1)]
//void main(uint3 dispatchThreadId : SV_DispatchThreadID)
//{
//    uint vertexIndex = dispatchThreadId.x;
//    uint objectId = vertexIndex / OBJECT_VERTEX_COUNT; // ������Ʈ �ε��� ���
//    uint vertexId = vertexIndex % OBJECT_VERTEX_COUNT; // ������Ʈ �� ���� �ε��� ���
//
//    // ���� ��ġ, ���, UV �� ������ ������
//    VertexInput inputVertex = inputBuffer[vertexIndex];
//    float3 position = inputVertex.position;
//
//    // ������Ʈ ������ ������
//    ObjectData objectData = objectBuffer[objectId];
//    float4x4 objectToWorld = objectData.objectToWorld;
//    float4x4 worldToObject = objectData.worldToObject;
//
//    // ������Ʈ ���������� ���� ��ġ ���
//    position = mul(position, objectToWorld);
//
//    // �浹 üũ
//    bool isCollided = CheckCollision(position, objectData.collisionData);
//
//    // �浹�� ��� ���� ��ġ ����
//    if (isCollided)
//    {
//        position += CalculateVertexDisplacement(position, objectData.collisionData);
//    }
//
//    // ���� �������� ��ȯ
//    position = mul(position, worldToObject);
//
//    // ���� ���� ������Ʈ �� ���ۿ� ����
//    VertexOutput outputVertex;
//    outputVertex.position = position;
//    outputVertex.normal = inputVertex.normal;
//    outputVertex.uv = inputVertex.uv;
//
//    outputBuffer[vertexIndex] = outputVertex;
//}



//struct Vertex {
//    float3 position : POSITION;
//    float3 normal   : NORMAL;
//};
//
//StructuredBuffer<Vertex> object1Vertices;
//StructuredBuffer<Vertex> object2Vertices;
//RWStructuredBuffer<int> collisionResult;
//
//[numthreads(16, 16, 1)]
//void CSMain(uint3 DTid : SV_DispatchThreadID) {
//    // ������Ʈ 1�� ������Ʈ 2�� ���� ������ ������
//    Vertex v1 = object1Vertices[DTid.x];
//    Vertex v2 = object2Vertices[DTid.y];
//
//    // �浹 �˻� �˰����� �����Ͽ� �浹�� ������ 1, ������ 0���� ����
//    int result = 0;
//    if (/* ������ �浹 �˻� �˰��� */) {
//        result = 1;
//    }
//
//    // �浹 ����� ���ۿ� ���
//    collisionResult[DTid.y * object1Vertices.Count + DTid.x] = result;
//}