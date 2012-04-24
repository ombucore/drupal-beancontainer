<?php

/**
 * @file
 * Bean plugin object for bean container.
 */

class BeanContainer extends BeanPlugin {
  /**
   * @see: parent::values().
   */
  public function values() {
    $values = parent::values();

    $values['children'] = array();

    return $values;
  }

  /**
   * Return the block content.
   *
   * @param $bean
   *   The bean object being viewed.
   * @param $content
   *   The default content array created by Entity API.  This will include any
   *   fields attached to the entity.
   * @param $view_mode
   *   The view mode passed into $entity->view().
   * @return
   *   Return a renderable content array.
   */
  public function view($bean, $content, $view_mode = 'default', $langcode = NULL) {
    $children = array();

    if (!empty($bean->children)) {
      $i = 1;
      foreach ($bean->children as $child) {
        $child_bean = bean_load($child);
        if (bean_access('view', $child_bean)) {
          $children[] = $child_bean;
        }
      }
    }

    $content['bean'][$bean->bid]['children'] = array(
      '#theme' => 'bean_container',
      '#children' => $children,
    );

    return $content;
  }
}
