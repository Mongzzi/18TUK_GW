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
//StructuredBuffer<VertexInput> inputBuffer : register(t0); // 정점 버퍼
//RWStructuredBuffer<VertexOutput> outputBuffer : register(u0); // 정점 변환 후 버퍼
//StructuredBuffer<ObjectData> objectBuffer : register(t1); // 오브젝트 정보 버퍼

//[numthreads(64, 1, 1)]
//void main(uint3 dispatchThreadId : SV_DispatchThreadID)
//{
//    uint vertexIndex = dispatchThreadId.x;
//    uint objectId = vertexIndex / OBJECT_VERTEX_COUNT; // 오브젝트 인덱스 계산
//    uint vertexId = vertexIndex % OBJECT_VERTEX_COUNT; // 오브젝트 내 정점 인덱스 계산
//
//    // 정점 위치, 노멀, UV 등 정보를 가져옴
//    VertexInput inputVertex = inputBuffer[vertexIndex];
//    float3 position = inputVertex.position;
//
//    // 오브젝트 정보를 가져옴
//    ObjectData objectData = objectBuffer[objectId];
//    float4x4 objectToWorld = objectData.objectToWorld;
//    float4x4 worldToObject = objectData.worldToObject;
//
//    // 오브젝트 공간에서의 정점 위치 계산
//    position = mul(position, objectToWorld);
//
//    // 충돌 체크
//    bool isCollided = CheckCollision(position, objectData.collisionData);
//
//    // 충돌한 경우 정점 위치 조정
//    if (isCollided)
//    {
//        position += CalculateVertexDisplacement(position, objectData.collisionData);
//    }
//
//    // 월드 공간으로 변환
//    position = mul(position, worldToObject);
//
//    // 정점 정보 업데이트 및 버퍼에 쓰기
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
//    // 오브젝트 1과 오브젝트 2의 정점 정보를 가져옴
//    Vertex v1 = object1Vertices[DTid.x];
//    Vertex v2 = object2Vertices[DTid.y];
//
//    // 충돌 검사 알고리즘을 적용하여 충돌이 있으면 1, 없으면 0으로 설정
//    int result = 0;
//    if (/* 엄격한 충돌 검사 알고리즘 */) {
//        result = 1;
//    }
//
//    // 충돌 결과를 버퍼에 기록
//    collisionResult[DTid.y * object1Vertices.Count + DTid.x] = result;
//}