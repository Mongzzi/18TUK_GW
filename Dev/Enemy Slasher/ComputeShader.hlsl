[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
}


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