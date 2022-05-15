#include "../RenderHelp/RenderHelp.h"

int main() {
	RenderHelp rh(800, 600);

	Bitmap texture(256, 256);
	for (int y = 0; y < 256; y++) {
		for (int x = 0; x < 256; x++) {
			int k = (x / 32 + y / 32) & 1;
			texture.SetPixel(x, y, k ? 0xffffffff : 0xff3fbcef);
		}
	}

	Mat4x4f mat_model = matrix_set_identity();
	Mat4x4f mat_view = matrix_set_lookat({ -0.7, 0, 1.5 }, { 0,0,0 }, { 0,0,1 });
	Mat4x4f mat_proj = matrix_set_perspective(3.1415926f * 0.5f, 800 / 600.0, 1.0, 500.0f);
	Mat4x4f mat_mvp = mat_model * mat_view * mat_proj;	// �ۺϱ任����

	struct VertexAttrib { Vec4f pos; Vec2f texuv; } vs_input[3];

	const int VARYING_TEXUV = 0;

		// ������ɫ��
	rh.SetVertexShader([&] (int index, ShaderContext& output) {
			Vec4f pos = vs_input[index].pos * mat_mvp;	// ����任�������
			output.varying_vec2f[VARYING_TEXUV] = vs_input[index].texuv;
			return pos;
		});

	// ������ɫ��
	rh.SetPixelShader([&] (ShaderContext& input) -> Vec4f {
			Vec2f coord = input.varying_vec2f[VARYING_TEXUV];	// ȡ����������
			return texture.Sample2D(coord);		// �������������������ɫ
		});

	// 0 1
	// 3 2  �������������Σ����һ������
	VertexAttrib vertex[] = {
		{ { 1, -1, -1, 1}, {0, 0} },
		{ { 1,  1, -1, 1}, {1, 0} },
		{ {-1,  1, -1, 1}, {1, 1} },
		{ {-1, -1, -1, 1}, {0, 1} },
	};

	vs_input[0] = vertex[0];
	vs_input[1] = vertex[1];
	vs_input[2] = vertex[2];
	rh.DrawPrimitive();

	vs_input[0] = vertex[2];
	vs_input[1] = vertex[3];
	vs_input[2] = vertex[0];
	rh.DrawPrimitive();

	// �����ļ�
	rh.SaveFile("output.bmp");

	// �û�����ʾͼƬ
#if defined(_WIN32) || defined(WIN32)
	system("mspaint.exe output.bmp");
#endif

	return 0;
}

