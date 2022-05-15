#include "../RenderHelp/RenderHelp.h"

int main() {
	// ��ʼ����Ⱦ����֡�����С
	RenderHelp rh(800, 600);

	const int VARYING_COLOR = 0;    // ����һ�� varying �� key

	// �������ݣ��� VS ��ȡ�����ж�������Σ���ÿ�θ��� vs_input �ٻ���
	struct { Vec4f pos; Vec4f color; } vs_input[3] = {
		{ {  0.0,  0.7, 0.90, 1}, {1, 0, 0, 1} },
		{ { -0.6, -0.2, 0.01, 1}, {0, 1, 0, 1} },
		{ { +0.6, -0.2, 0.01, 1}, {0, 0, 1, 1} },
	};

	// ������ɫ������ʼ�� varying ���������꣬
	// ���� index ����Ⱦ������Ķ�����ţ���Χ [0, 2] ���ڶ�ȡ��������
	rh.SetVertexShader([&] (int index, ShaderContext& output) -> Vec4f {
			output.varying_vec4f[VARYING_COLOR] = vs_input[index].color;
			return vs_input[index].pos;		// ֱ�ӷ�������
		});

	// ������ɫ����������ɫ
	rh.SetPixelShader([&] (ShaderContext& input) -> Vec4f {
			return input.varying_vec4f[VARYING_COLOR];
		});

	// ��Ⱦ������
	rh.DrawPrimitive();
	rh.SaveFile("output.bmp");

	// �û�����ʾͼƬ
#if defined(_WIN32) || defined(WIN32)
	system("mspaint.exe output.bmp");
#endif

	return 0;
}