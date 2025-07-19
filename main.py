# -*- coding: utf-8 -*-
"""
Visualizador de Logs para Bancada de Caracterização de Propulsores
Versão: 1.0
Data: 07/2025
Autor: Euler Torres
"""
import sys
import pandas as pd
import numpy as np
import pyqtgraph as pg
from pyqtgraph.opengl import GLViewWidget, GLGridItem, GLAxisItem, GLScatterPlotItem

from PyQt6.QtWidgets import (
    QApplication, QMainWindow, QWidget, QVBoxLayout, QHBoxLayout,
    QPushButton, QFileDialog, QLabel, QComboBox, QStackedWidget, QMessageBox
)
from PyQt6.QtCore import Qt, QSize
from PyQt6.QtGui import QFont

# Estilo QSS (similar ao CSS) para o tema dark moderno
DARK_STYLESHEET = """
QWidget {
    background-color: #1e1e1e;
    color: #dcdcdc;
    font-family: 'Segoe UI', Arial, sans-serif;
    font-size: 14px;
}
QMainWindow {
    border: 1px solid #3c3c3c;
}
QPushButton {
    background-color: #3a3a3a;
    border: 1px solid #555555;
    padding: 8px 16px;
    border-radius: 4px;
}
QPushButton:hover {
    background-color: #4a4a4a;
    border-color: #6a6a6a;
}
QPushButton:pressed {
    background-color: #2a2a2a;
}
QComboBox {
    background-color: #2c2c2c;
    border: 1px solid #555555;
    border-radius: 4px;
    padding: 5px;
    min-width: 6em;
}
QComboBox::drop-down {
    subcontrol-origin: padding;
    subcontrol-position: top right;
    width: 20px;
    border-left-width: 1px;
    border-left-color: #555555;
    border-left-style: solid;
    border-top-right-radius: 3px;
    border-bottom-right-radius: 3px;
}
QComboBox QAbstractItemView {
    background-color: #2c2c2c;
    border: 1px solid #555555;
    selection-background-color: #007acc;
}
QLabel {
    padding: 2px;
}
#TitleLabel {
    font-size: 20px;
    font-weight: bold;
    color: #00aaff;
    padding-bottom: 10px;
}
#InfoLabel {
    font-size: 12px;
    color: #999999;
    padding: 8px;
    background-color: #2a2a2a;
    border-radius: 4px;
}
"""

class PropellerTestbedViewer(QMainWindow):
    def __init__(self):
        super().__init__()
        self.data_frame = None
        self.scatter_item = None
        self.trend_item = None
        self.hover_label = None

        self.setWindowTitle("🚀 Visualizador de Logs da Bancada de Testes")
        self.setGeometry(100, 100, 1400, 800)
        
        self.central_widget = QWidget()
        self.setCentralWidget(self.central_widget)
        
        # Aplicando o tema dark
        self.setStyleSheet(DARK_STYLESHEET)
        pg.setConfigOption('background', '#1e1e1e')
        pg.setConfigOption('foreground', '#dcdcdc')

        self._setup_ui()

    def _setup_ui(self):
        """Configura a interface do usuário (widgets e layouts)."""
        main_layout = QHBoxLayout(self.central_widget)
        
        # --- Painel de Controle (Esquerda) ---
        control_panel = QWidget()
        control_layout = QVBoxLayout(control_panel)
        control_panel.setFixedWidth(300)

        title_label = QLabel("Painel de Controle")
        title_label.setObjectName("TitleLabel")
        title_label.setAlignment(Qt.AlignmentFlag.AlignCenter)

        self.load_button = QPushButton("📂 Carregar Log CSV")
        self.load_button.clicked.connect(self.load_csv_file)
        self.file_label = QLabel("Nenhum arquivo carregado.")
        self.file_label.setWordWrap(True)

        # --- INÍCIO DA CORREÇÃO ---
        x_axis_widget, self.x_axis_combo = self._create_axis_selector("Eixo X:")
        y_axis_widget, self.y_axis_combo = self._create_axis_selector("Eixo Y:")
        z_axis_widget, self.z_axis_combo = self._create_axis_selector("Eixo Z (para 3D):")
        
        self.plot_2d_button = QPushButton("📊 Gerar Gráfico 2D")
        self.plot_2d_button.clicked.connect(self.plot_2d)
        self.plot_3d_button = QPushButton("🌐 Gerar Mapa 3D")
        self.plot_3d_button.clicked.connect(self.plot_3d)

        self.hover_info_label = QLabel("Passe o mouse sobre um ponto para ver os detalhes.")
        self.hover_info_label.setObjectName("InfoLabel")
        self.hover_info_label.setMinimumHeight(150)
        self.hover_info_label.setAlignment(Qt.AlignmentFlag.AlignTop)
        self.hover_info_label.setWordWrap(True)
        
        control_layout.addWidget(title_label)
        control_layout.addWidget(self.load_button)
        control_layout.addWidget(self.file_label)
        control_layout.addSpacing(20)
        
        # Adicionamos os widgets contêineres, não o .parent()
        control_layout.addWidget(x_axis_widget)
        control_layout.addWidget(y_axis_widget)
        control_layout.addWidget(z_axis_widget)
        
        control_layout.addSpacing(20)
        control_layout.addWidget(self.plot_2d_button)
        control_layout.addWidget(self.plot_3d_button)
        control_layout.addStretch()
        control_layout.addWidget(QLabel("Informações do Ponto:"))
        control_layout.addWidget(self.hover_info_label)
        # --- FIM DA CORREÇÃO ---

        # --- Área de Plotagem (Direita) ---
        plot_area = QWidget()
        plot_layout = QVBoxLayout(plot_area)
        self.plot_stack = QStackedWidget()
        self.plot_widget_2d = pg.GraphicsLayoutWidget()
        self.plot_widget_3d = GLViewWidget()
        self.plot_stack.addWidget(self.plot_widget_2d)
        self.plot_stack.addWidget(self.plot_widget_3d)
        plot_layout.addWidget(self.plot_stack)

        main_layout.addWidget(control_panel)
        main_layout.addWidget(plot_area)
        
        self._update_button_states()

    def _create_axis_selector(self, label_text):
        """Cria um conjunto de QLabel e QComboBox e retorna ambos."""
        # --- INÍCIO DA CORREÇÃO ---
        widget = QWidget()
        layout = QHBoxLayout(widget)
        layout.setContentsMargins(0, 0, 0, 0)
        label = QLabel(label_text)
        combo = QComboBox()
        layout.addWidget(label)
        layout.addWidget(combo)
        return widget, combo # Retorna o widget contêiner E a combo box

    def _update_button_states(self):
        """Habilita ou desabilita os botões com base no estado dos dados."""
        has_data = self.data_frame is not None
        self.plot_2d_button.setEnabled(has_data)
        self.plot_3d_button.setEnabled(has_data)
        self.x_axis_combo.setEnabled(has_data)
        self.y_axis_combo.setEnabled(has_data)
        self.z_axis_combo.setEnabled(has_data)
        
    def load_csv_file(self):
        """Abre um diálogo para selecionar e carregar um arquivo CSV."""
        file_path, _ = QFileDialog.getOpenFileName(
            self, "Selecionar Arquivo de Log", "", "Arquivos CSV (*.csv);;Todos os Arquivos (*)"
        )
        if file_path:
            try:
                self.data_frame = pd.read_csv(file_path).dropna()
                self.file_label.setText(f"Arquivo: ...{file_path[-40:]}")
                
                # Preenche os ComboBoxes com as colunas do CSV
                columns = self.data_frame.columns.tolist()
                for combo in [self.x_axis_combo, self.y_axis_combo, self.z_axis_combo]:
                    combo.clear()
                    combo.addItems(columns)
                
                # Sugere eixos comuns por padrão
                if 'PWM[us]' in columns: self.x_axis_combo.setCurrentText('PWM[us]')
                if 'Empuxo' in columns: self.y_axis_combo.setCurrentText('Empuxo')
                if 'RPM' in columns: self.z_axis_combo.setCurrentText('RPM')

            except Exception as e:
                QMessageBox.critical(self, "Erro de Leitura", f"Não foi possível ler o arquivo:\n{e}")
                self.data_frame = None
                self.file_label.setText("Falha ao carregar o arquivo.")

            self._update_button_states()

    def plot_2d(self):
        """Gera o gráfico 2D com os eixos selecionados."""
        if self.data_frame is None: return

        self.plot_stack.setCurrentWidget(self.plot_widget_2d)
        
        x_col = self.x_axis_combo.currentText()
        y_col = self.y_axis_combo.currentText()

        if not x_col or not y_col:
            QMessageBox.warning(self, "Seleção Inválida", "Por favor, selecione os eixos X e Y.")
            return

        x_data = self.data_frame[x_col].values
        y_data = self.data_frame[y_col].values

        # Limpa o gráfico anterior
        self.plot_widget_2d.clear()
        
        # Adiciona um novo plot
        p1 = self.plot_widget_2d.addPlot(row=0, col=0)
        p1.setLabel('bottom', x_col)
        p1.setLabel('left', y_col)
        p1.setTitle(f"{y_col} vs. {x_col}", color="#dcdcdc", size="12pt")
        p1.showGrid(x=True, y=True, alpha=0.3)
        p1.addLegend()

        # Plota os pontos (scatter plot)
        self.scatter_item = pg.ScatterPlotItem(
            x=x_data, y=y_data, 
            pen=None, 
            symbol='o', 
            size=8, 
            brush=pg.mkBrush(0, 170, 255, 180),
            hoverable=True,
            hoverSymbol='s',
            hoverSize=12,
            hoverBrush=pg.mkBrush("#00aaff"),
            name='Dados Medidos'
        )
        # Adiciona o índice original a cada ponto para busca no DataFrame
        for i, point in enumerate(self.scatter_item.points()):
            point.setData(i)

        self.scatter_item.sigHovered.connect(self.on_hover)
        p1.addItem(self.scatter_item)

        # Calcula e plota a linha de tendência (regressão linear de 2ª ordem)
        try:
            coeffs = np.polyfit(x_data, y_data, 2)
            poly = np.poly1d(coeffs)
            x_fit = np.linspace(x_data.min(), x_data.max(), 200)
            y_fit = poly(x_fit)
            self.trend_item = p1.plot(x_fit, y_fit, pen=pg.mkPen('r', width=2, style=Qt.PenStyle.DashLine), name='Tendência (Polinomial 2º)')
        except np.linalg.LinAlgError:
            print("Não foi possível calcular a linha de tendência.")


    def plot_3d(self):
        """Gera o mapa 3D com os eixos selecionados."""
        if self.data_frame is None: return

        self.plot_stack.setCurrentWidget(self.plot_widget_3d)

        x_col = self.x_axis_combo.currentText()
        y_col = self.y_axis_combo.currentText()
        z_col = self.z_axis_combo.currentText()
        
        if not all([x_col, y_col, z_col]):
            QMessageBox.warning(self, "Seleção Inválida", "Por favor, selecione os eixos X, Y e Z.")
            return

        x = self.data_frame[x_col].values
        y = self.data_frame[y_col].values
        z = self.data_frame[z_col].values

        # Limpa a visualização 3D anterior
        self.plot_widget_3d.clear()
        self.plot_widget_3d.opts['distance'] = (x.max() + y.max() + z.max()) # Ajusta zoom inicial
        self.plot_widget_3d.setCameraPosition(azimuth=45, elevation=30)


        # Adiciona grade e eixos
        grid = GLGridItem()
        grid.scale(20, 20, 1)
        self.plot_widget_3d.addItem(grid)
        
        axis = GLAxisItem()
        axis.setSize(x=x.max()*1.1, y=y.max()*1.1, z=z.max()*1.1)
        self.plot_widget_3d.addItem(axis)

        # Cria a nuvem de pontos 3D
        pos = np.vstack((x, y, z)).transpose()
        
        # Cores baseadas no valor de Z
        colors = pg.glColorMap(z, 'viridis')
        
        scatter = GLScatterPlotItem(pos=pos, color=colors, size=8, pxMode=True)
        self.plot_widget_3d.addItem(scatter)
        
        # Limpa o label de hover, pois não há hover no 3D
        self.hover_info_label.setText("Visualização 3D ativa.\n\nControles:\n- Roda do mouse: Zoom\n- Botão esquerdo: Girar\n- Botão direito: Mover")


    def on_hover(self, _, points):
        """Callback acionado quando o mouse passa sobre um ponto do gráfico 2D."""
        if points:
            point = points[0]
            point_index = point.data() # Recupera o índice do DataFrame
            
            point_data = self.data_frame.iloc[point_index]
            
            # Formata o texto de informações
            info_text = ""
            for col, val in point_data.items():
                if isinstance(val, float):
                    info_text += f"<b>{col}:</b> {val:.2f}\n"
                else:
                    info_text += f"<b>{col}:</b> {val}\n"
            
            self.hover_info_label.setText(info_text.strip())
        else:
            self.hover_info_label.setText("Passe o mouse sobre um ponto para ver os detalhes.")


if __name__ == '__main__':
    app = QApplication(sys.argv)
    window = PropellerTestbedViewer()
    window.show()
    sys.exit(app.exec())